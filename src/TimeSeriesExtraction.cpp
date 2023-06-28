#include "TimeSeriesExtraction.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <StringProcessing.h>
#include <CallbackData.h>

TimeSeriesExtraction::TimeSeriesExtraction(const std::string path, const std::string kws,
                                               BitermWeight *bw, CandidateIdentification *ci, TopicIdentification *ti):_scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
    _bw = bw;
    _ci = ci;
    _ti = ti;
}

TimeSeriesExtraction::~TimeSeriesExtraction()
{
    //dtor
}

bool TimeSeriesExtraction::finished()
{
    if (!load(_y2, NULL))
        return false;
    if (!load(_y2 + 5, NULL))
        return false;
    return true;
}

const char *TimeSeriesExtraction::name()
{
    return "Time Series Extraction";
}

int TimeSeriesExtraction::numSteps()
{
    return 2;
}

void TimeSeriesExtraction::doStep(int stepId)
{
    if (stepId == 0)
        process(_y2);
    else
        process(_y2 + 5);
}

bool TimeSeriesExtraction::load(int y, std::map<uint64_t, std::pair<std::vector<int>,std::vector<int>>> *timeSeries)
{
    GeneralConfig config;
    std::string path = config.getDatabase();

    sqlite3 *db = NULL;
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + path));
        return false;
    }
    char *errorMessage = NULL;

    // step 1: load time series
    std::string keywords = _scope.getKeywords();
    if (timeSeries != NULL)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT id, scope_keywords, year, lts, rts FROM pub_scope_time_series WHERE scope_keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        for (auto result: data.results)
        {
            uint64_t id = std::stoull(result["id"]);
            std::vector<int> lts = getIntVector(result["lts"]);
            std::vector<int> rts = getIntVector(result["rts"]);
            std::pair<std::vector<int>,std::vector<int>> tsm(lts,rts);
            (*timeSeries)[id] = tsm;
        }
    }

    // step 2: load token
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_time_series_token WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        if (data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }
    }
    sqlite3_close(db);
    return true;
}

bool TimeSeriesExtraction::save(int y, const std::map<uint64_t, std::pair<std::vector<int>,std::vector<int>>> &timeSeries)
{
    GeneralConfig config;
    std::string path = config.getDatabase();

    sqlite3 *db = NULL;
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + path));
        return false;
    }
    char *errorMessage = NULL;

    // step 1: create tables
    const char*sqls[] =
    {
        "CREATE TABLE IF NOT EXISTS scope_time_series_token("
        "keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "PRIMARY KEY(keywords,year));",

        "CREATE TABLE IF NOT EXISTS pub_scope_time_series("
        "id INTEGER,"
        "scope_keywords TEXT,"
        "year INTEGER,"
        "lts TEXT,"
        "rts TEXT,"
        "PRIMARY KEY(id,scope_keywords,year))",
    };
    for (const char*sql: sqls)
    {
        logDebug(sql);
        rc = sqlite3_exec(db, sql, NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    // step 2: insert publication scope time series
    std::string keywords = _scope.getKeywords();
    time_t t;
    time(&t);
    if (timeSeries.size() > 0)
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO pub_scope_time_series(id, scope_keywords, year, lts, rts) VALUES ";
        for (auto idToTS = timeSeries.begin(); idToTS != timeSeries.end(); idToTS++)
        {
            uint64_t id = idToTS->first;
            std::vector<int> lts = idToTS->second.first;
            std::vector<int> rts = idToTS->second.second;
            std::string strLts = getVectorStr(lts);
            std::string strRts = getVectorStr(rts);
            if (idToTS != timeSeries.begin())
                ss << ",";
            ss << "(" << id << ",'" << keywords << "'," << y << ",'" << strLts << "','" << strRts << "')";
        }
        ss << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    // step 3: insert token
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_time_series_token(keywords,year,update_time) VALUES ('"
            << keywords << "'," << y << "," << (int) t << ");";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    sqlite3_close(db);
    return true;
}

bool TimeSeriesExtraction::process(int y)
{
    if (load(y, NULL))
        return true;

    // step 1: load candidates
    std::map<uint64_t, std::pair<std::vector<int>,std::vector<int>>> timeSeries;
    std::map<uint64_t, int> candidateMap;
    {
        std::vector<uint64_t> candidates;
        if (!_ci->load(y, &candidates))
            return false;
        for (int i = 0; i < (int)candidates.size(); i++)
        {
            candidateMap[candidates[i]] = i;
        }
    }
    if (candidateMap.size() == 0)
    {
        save(y, timeSeries);
        return true;
    }
    GeneralConfig config;
    int numCandidates = (int) candidateMap.size();
    int hits[numCandidates][15];
    for (int iC = 0; iC < numCandidates; iC++)
    {
        for (int iY = 0; iY < 15; iY++)
        {
            hits[iC][iY] = 0;
        }
    }

    // step 2: load topics
    std::map<uint64_t,std::pair<std::string,std::string>> topics;
    if (!_ti->load(y, &topics))
        return false;

    // step 3: create mapping that maps biterm to candidates
    std::map<std::string, std::vector<uint64_t>> bitermCandidatePositions;
    for (auto cidToTopic: topics)
    {
        uint64_t cid = cidToTopic.first;
        std::vector<std::string> biterms = splitString(cidToTopic.second.first, "|");
        for (int i = 0; i < (int)biterms.size(); i++)
        {
            auto bToC = bitermCandidatePositions.find(biterms[i]);
            if (bToC == bitermCandidatePositions.end())
            {
                std::vector<uint64_t> temp;
                temp.push_back(cid);
                bitermCandidatePositions[biterms[i]] = temp;
            }
            else
            {
                bitermCandidatePositions[biterms[i]].push_back(cid);
            }
        }
    }

    // step 4: scan through publications to find biterm hits
    for (int i = 0; i < 15; i++)
    {
        int yi = y - 1 - i;
        if (yi >= WESTSeerApp::year())
            continue;
        int iY = 14 - i;

        std::map<uint64_t, std::vector<uint64_t>> pubs;
        if (!_scope.getExistingRefIds(yi, pubs))
            return false;

        std::map<uint64_t, std::map<std::string, double>> pubBWs;
        if (!_bw->load(yi, &pubBWs))
            return false;

        for (auto &idToBWs: pubBWs)
        {
            std::set<uint64_t> refs(pubs[idToBWs.first].begin(), pubs[idToBWs.first].end());

            // calculate pHits
            std::set<uint64_t> pHits;
            for (auto &bToW: idToBWs.second)
            {
                auto bToC = bitermCandidatePositions.find(bToW.first);
                if (bToC != bitermCandidatePositions.end())
                {
                    for (uint64_t cid: bToC->second)
                    {
                        // update pHits
                        if (refs.find(cid) != refs.end())
                        {
                            pHits.insert(cid);
                        }
                    }
                }
            }

            // update hits according to pHits
            for (uint64_t cid: pHits)
            {
                int iC = candidateMap[cid];
                hits[iC][iY]++;
            }
        }
    }

    // step 5: encode hits to time series
    for (auto idToiC: candidateMap)
    {
        uint64_t id = idToiC.first;
        int iC = idToiC.second;
        std::vector<int> lts(10), rts(5);
        for (int iY = 0; iY < 10; iY++)
        {
            lts[iY] = hits[iC][iY];
        }
        for (int iY = 10; iY < 15; iY++)
        {
            rts[iY-10] = hits[iC][iY];
        }
        std::pair<std::vector<int>,std::vector<int>> tsm(lts,rts);
        timeSeries[id] = tsm;
    }

    // step 8: save time series
    save(y, timeSeries);
    return true;
}