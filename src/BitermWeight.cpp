#include "BitermWeight.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <CallbackData.h>
#include <StringProcessing.h>
#include <TimeSeriesRegression.h>
#include <queue>
#include <thread>
#include <mutex>

BitermWeight::BitermWeight(const std::string path, const std::string kws, TermTfIrdf *tt, BitermDf *bdf) : _scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
    _tt = tt;
    _bdf = bdf;
}

BitermWeight::~BitermWeight()
{
    //dtor
}

bool BitermWeight::finished()
{
    for (int y = _y0; y < _y2; y++)
    {
        if (!load(y, NULL))
            return false;
    }
    return true;
}

const char *BitermWeight::name()
{
    return "Biterm Weight Calculation.";
}

int BitermWeight::numSteps()
{
    return _y2 - _y0;
}

void BitermWeight::doStep(int stepId)
{
    process(_y0 + stepId);
}

bool BitermWeight::load(const std::string keywords, int y, std::map<uint64_t, std::map<std::string, double>> *bitermWeights)
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
    CallbackData data;
    char *errorMessage = NULL;

    // step 1: load scope bitermWeights
    if (bitermWeights != NULL)
    {
        bitermWeights->clear();
        {
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, year, biterm_weights FROM pub_scope_bws WHERE scope_keywords = '"
                << keywords << "' AND year = " << y << ";";
            logDebug(ss.str().c_str());
            rc = sqlite3_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
            if (rc != SQLITE_OK)
            {
                logDebug(errorMessage);
                sqlite3_close(db);
                return false;
            }
            for (auto &result: data.results)
            {
                uint64_t id = std::stoull(result["id"]);
                std::map<std::string, double> bwsOfId;
                std::vector<std::string> strBWs = splitString(result["biterm_weights"], ",");
                for (std::string strBW : strBWs)
                {
                    std::vector<std::string> fields = splitString(strBW, ":");
                    bwsOfId[fields[0]] = atof(fields[1].c_str());
                }
                (*bitermWeights)[id] = bwsOfId;
            }
        }
    }

    // step 2: load token
    data.results.clear();
    {
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_bw_tokens WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }
    sqlite3_close(db);
    return data.results.size() > 0;
}

bool BitermWeight::load(int y, std::map<uint64_t, std::map<std::string, double>> *bitermWeights)
{
    return load(_scope.getKeywords(), y, bitermWeights);
}

bool BitermWeight::save(int y, std::map<uint64_t, std::map<std::string, double>> &bitermWeights)
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
        "CREATE TABLE IF NOT EXISTS pub_scope_bws("
        "id INTEGER,"
        "scope_keywords TEXT,"
        "year INTEGER,"
        "biterm_weights TEXT,"
        "PRIMARY KEY(id,scope_keywords))",

        "CREATE TABLE IF NOT EXISTS scope_bw_tokens("
        "keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "PRIMARY KEY(keywords,year))",
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

    // step 2: save publication biterm weights
    std::string keywords = _scope.getKeywords();
    if (bitermWeights.size() > 0)
    {
        std::vector<uint64_t> ids;
        for (auto idToBW = bitermWeights.begin(); idToBW != bitermWeights.end(); idToBW++)
        {
            ids.push_back(idToBW->first);
        }

        for (size_t i0 = 0; i0 < ids.size(); i0 += 1000)
        {
            size_t i1 = i0 + 1000;
            if (i1 > ids.size())
                i1 = ids.size();

            std::stringstream ss;
            ss << "INSERT OR IGNORE INTO pub_scope_bws(id, scope_keywords, year, biterm_weights) VALUES ";
            for (size_t i = i0; i < i1; i++)
            {
                auto idToBW = bitermWeights.find(ids[i]);
                if (i > i0)
                    ss << ",";
                ss << "(" << idToBW->first << ",'" << keywords << "'," << y << ",'";
                for (auto bToW = idToBW->second.begin(); bToW != idToBW->second.end(); bToW++)
                {
                    if (bToW != idToBW->second.begin())
                        ss << ",";
                    ss << bToW->first << ":" << bToW->second;
                }
                ss << "')";
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

    }

    // step 3: save scope bw token
    {
        time_t t;
        time(&t);
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_bw_tokens(keywords, year, update_time) VALUES ('"
            << keywords << "'," << y << "," << (int)t << ");";
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

bool BitermWeight::process(int y)
{
    if (load(y, NULL))
        return true;

    // step 1: load tfirdfs
    std::map<uint64_t, std::map<std::string, double>> tfirdfs;
    if (!_tt->load(y, &tfirdfs, false))
        return false;

    // step 2: load bitermDfs
    std::map<std::string, int> bdfs[10];
    int numPubs = 0;
    for (int i = 0; i < 10; i++)
    {
        _bdf->load(y - i, &(bdfs[9 - i]));
        numPubs += _scope.numPublications(y - i);
        if (_cancelled.load() == true)
        {
            return false;
        }
    }

    // step 3: compute mean tfirdf in publication
    std::map<uint64_t, double> meanTfirdfs;
    int nThreads = std::thread::hardware_concurrency();
    std::queue<uint64_t> q;
    for (auto idToWorkTfirdfs = tfirdfs.begin(); idToWorkTfirdfs != tfirdfs.end(); idToWorkTfirdfs++)
    {
        q.push(idToWorkTfirdfs->first);
    }
    std::mutex mq;
    std::thread *threads[nThreads];
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread(
            [&q, &mq, &tfirdfs, &meanTfirdfs, this]
            {
                for (;;)
                {
                    uint64_t id = 0;
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        if (!q.empty())
                        {
                            id = q.front();
                            q.pop();
                        }
                        else
                            return;
                    }
                    auto idToWorkTfirdfs = tfirdfs.find(id);
                    if (idToWorkTfirdfs == tfirdfs.end())
                        return;
                    double sumTfirdfs = 0.0;
                    int n = 0;
                    for (auto termToTfirdf = idToWorkTfirdfs->second.begin(); termToTfirdf != idToWorkTfirdfs->second.end(); termToTfirdf++)
                    {
                        sumTfirdfs += termToTfirdf->second;
                        n++;
                    }
                    double meanTfirdf = sumTfirdfs / n;
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        meanTfirdfs[idToWorkTfirdfs->first] = meanTfirdf;
                    }
                    if (_cancelled.load() == true)
                    {
                        return;
                    }
                }
            });
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid]->join();
        delete threads[tid];
    }

    if (meanTfirdfs.size() < tfirdfs.size())
        return false;

    // step 4: compute biterm weights
    std::map<uint64_t, std::map<std::string, double>> bitermWeights;
    for (auto idToWorkTfirdfs = tfirdfs.begin(); idToWorkTfirdfs != tfirdfs.end(); idToWorkTfirdfs++)
    {
        q.push(idToWorkTfirdfs->first);
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread(
            [&q,&mq,&tfirdfs,&meanTfirdfs,&bdfs,&bitermWeights,this,numPubs]
            {
                for (;;)
                {
                    uint64_t id = 0;
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        if (!q.empty())
                        {
                            id = q.front();
                            q.pop();
                        }
                        else
                            break;
                    }
                    auto idToTTf = tfirdfs.find(id);
                    if (idToTTf == tfirdfs.end())
                        return;
                    double mean = meanTfirdfs[id];

                    std::map<std::string, double> bwOfId;
                    for (auto tToTf1 = idToTTf->second.begin(); tToTf1 != idToTTf->second.end(); tToTf1++)
                    {
                        if (tToTf1->second < mean)
                            continue;
                        for (auto tToTf2 = idToTTf->second.begin(); tToTf2 != idToTTf->second.end(); tToTf2++)
                        {
                            if (tToTf2->second < mean)
                                continue;
                            if (tToTf1->first < tToTf2->first)
                            {
                                std::string biterm = tToTf1->first + "&" + tToTf2->first;
                                std::vector<double> df(10);
                                double sumDf = 0;
                                for (int i = 0; i < 10; i++)
                                {
                                    auto btToDf = bdfs[i].find(biterm);
                                    if (btToDf != bdfs[i].end())
                                    {
                                        df[i] = btToDf->second;
                                    }
                                    else
                                    {
                                        df[i] = 0;
                                    }
                                    sumDf += df[i];
                                }
                                if (sumDf <= 0)
                                    continue;

                                TimeSeriesRegression regression(df, TimeSeriesRegression::RegressionType::TP_QUADRATIC);
                                double cosErr = regression.cosError();
                                double slope = regression.slope(9);
                                double aSlope = slope > 0 ? std::atan(slope) :0;
                                double irdf = std::log(numPubs) - std::log(sumDf);
                                double w = cosErr * aSlope * irdf * tToTf1->second * tToTf2->second;
                                bwOfId[biterm] = w;
                            }
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lock(mq);
                        bitermWeights[id] = bwOfId;
                    }

                    if (_cancelled.load() == true)
                    {
                        return;
                    }
                }
            }
        );
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid]->join();
        delete threads[tid];
    }
    if (bitermWeights.size() < tfirdfs.size())
        return false;


    save(y, bitermWeights);
    return true;
}
