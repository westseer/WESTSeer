#include "TermTfIrdf.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <CallbackData.h>
#include <wxFFileLog.h>
#include <StringProcessing.h>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <set>
#include <thread>
#include <mutex>
#include <queue>

TermTfIrdf::TermTfIrdf(const std::string path, const std::string kws, TermExtraction *te) : _scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _te = te;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
    _numWorks = 0;
}

TermTfIrdf::~TermTfIrdf()
{
    //dtor
}

bool TermTfIrdf::finished()
{
    for (int y = _y2 - 1; y >= _y0; y--)
    {
        if (!load(y, NULL, false))
            return false;
    }
    return true;
}

const char *TermTfIrdf::name()
{
    return "Term TF-IRDF Calculation";
}

int TermTfIrdf::numSteps()
{
    return _y2 - _y0;
}

void TermTfIrdf::doStep(int stepId)
{
    process(_y0 + stepId);
}

bool TermTfIrdf::save(int y, const std::map<uint64_t, std::map<std::string, double>> &tfirdfs)
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
        "CREATE TABLE IF NOT EXISTS scope_dfs("
        "keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "num_works INTEGER,"
        "dfs TEXT,"
        "PRIMARY KEY(keywords,year));",

        "CREATE TABLE IF NOT EXISTS pub_scope_tfirdfs("
        "id INTEGER,"
        "scope_keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "tfirdfs TEXT,"
        "PRIMARY KEY(id,scope_keywords))",
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

    // step 2: save publication scope tfirdfs
    std::string keywords = _scope.getKeywords();
    time_t t;
    time(&t);
    if (tfirdfs.size() > 0)
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO pub_scope_tfirdfs(id, scope_keywords, year, update_time, tfirdfs) VALUES ";
        for (auto iter = tfirdfs.begin(); iter != tfirdfs.end(); iter++)
        {
            if (iter != tfirdfs.begin())
                ss << ",";
            ss << "(" << iter->first << ",'" << keywords << "'," << y << "," << (int)t << ",'";
            for (auto termToTfirdfs = iter->second.begin(); termToTfirdfs != iter->second.end(); termToTfirdfs++)
            {
                if (termToTfirdfs != iter->second.begin())
                    ss << ",";
                ss << termToTfirdfs->first << ":" << termToTfirdfs->second;
            }
            ss << "')";
        }
        ss << ";";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(tfirdfs.size(),strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    // step 3: save scope dfs
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_dfs(keywords, year, update_time, num_works, dfs) VALUES ('"
            << keywords << "'," << y << "," << (int)t << "," << _numWorks <<",'";
        for (auto iter = _dfs.begin(); iter != _dfs.end(); iter++)
        {
            if (iter != _dfs.begin())
                ss << ",";
            ss << iter->first << ":" << iter->second;
        }
        ss << "');";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(1, strSql.size());
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

bool TermTfIrdf::load(const std::string keywords, int y, std::map<uint64_t, std::map<std::string, double>> *tfirdfs)
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

    // step 1: load publication scope tfirdfs
    if (tfirdfs != NULL)
    {
        tfirdfs->clear();
        {
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, year, tfirdfs FROM pub_scope_tfirdfs WHERE scope_keywords = '"
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
                std::map<std::string, double> myTfirdfs;
                uint64_t id = std::stoull(result["id"]);
                std::string strTfirdfs = result["tfirdfs"];
                std::vector<std::string> fields = splitString(strTfirdfs, ",");
                for (std::string field: fields)
                {
                    std::vector<std::string> kv = splitString(field, ":");
                    myTfirdfs[kv[0]] = atof(kv[1].c_str());
                }
                (*tfirdfs)[id] = myTfirdfs;
            }
        }
    }
    sqlite3_close(db);
    return true;
}

bool TermTfIrdf::load(const std::string keywords, const std::set<uint64_t>& ids, std::map<uint64_t, std::map<std::string, double>> *tfirdfs)
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

    // step 1: load publication scope tfirdfs
    if (tfirdfs != NULL)
    {
        tfirdfs->clear();
        {
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, tfirdfs FROM pub_scope_tfirdfs WHERE scope_keywords = '"
                << keywords << "' AND id in (";
            for (auto iter = ids.begin(); iter != ids.end(); iter++)
            {
                if (iter != ids.begin())
                    ss << ",";
                ss << *iter;
            }
            ss << ");";
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
                std::map<std::string, double> myTfirdfs;
                uint64_t id = std::stoull(result["id"]);
                std::string strTfirdfs = result["tfirdfs"];
                std::vector<std::string> fields = splitString(strTfirdfs, ",");
                for (std::string field: fields)
                {
                    std::vector<std::string> kv = splitString(field, ":");
                    myTfirdfs[kv[0]] = atof(kv[1].c_str());
                }
                (*tfirdfs)[id] = myTfirdfs;
            }
        }
    }
    sqlite3_close(db);
    return true;
}

bool TermTfIrdf::load(int y, std::map<uint64_t, std::map<std::string, double>> *tfirdfs, bool loadDfs)
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

    // step 1: load publication scope tfirdfs
    std::string keywords = _scope.getKeywords();
    if (tfirdfs != NULL)
    {
        tfirdfs->clear();
        {
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, year, tfirdfs FROM pub_scope_tfirdfs WHERE scope_keywords = '"
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
                std::map<std::string, double> myTfirdfs;
                uint64_t id = std::stoull(result["id"]);
                std::string strTfirdfs = result["tfirdfs"];
                std::vector<std::string> fields = splitString(strTfirdfs, ",");
                for (std::string field: fields)
                {
                    std::vector<std::string> kv = splitString(field, ":");
                    myTfirdfs[kv[0]] = atof(kv[1].c_str());
                }
                (*tfirdfs)[id] = myTfirdfs;
            }
        }
    }


    // step 2: load scope dfs
    data.results.clear();
    if (loadDfs)
    {
        std::stringstream ss;
        ss << "SELECT keywords, year, num_works, dfs FROM scope_dfs WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        if (data.results.size() > 0)
        {
            _numWorks = atoi(data.results[0]["num_works"].c_str());

            std::string strDfs = data.results[0]["dfs"];
            std::vector<std::string> fields = splitString(strDfs, ",");
            for (std::string field: fields)
            {
                std::vector<std::string> kv = splitString(field, ":");
                _dfs[kv[0]] = atoi(kv[1].c_str());
            }
        }
    }
    else
    {
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_dfs WHERE keywords = '"
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
    return (data.results.size() > 0);
}

bool TermTfIrdf::process(int y)
{
    if (load(y + 1, NULL, false))
        return true;
    if (load(y, NULL, true))
        return true;

    // step 1: load term freqs
    std::map<uint64_t, std::map<std::string, std::pair<std::string,int>>> termFreqs;
    if (!_te->load(y, &termFreqs, false))
        return false;
    if (_cancelled.load() == true)
    {
        return false;
    }

    // step 2: update _dfs
    for (auto idToTF = termFreqs.begin(); idToTF != termFreqs.end(); idToTF++)
    {
        _numWorks++;
        for (auto termToFreq = idToTF->second.begin(); termToFreq != idToTF->second.end(); termToFreq++)
        {
            auto termToDf = _dfs.find(termToFreq->first);
            if (termToDf == _dfs.end())
            {
                _dfs[termToFreq->first] = 1;
            }
            else
            {
                _dfs[termToFreq->first] = termToDf->second + 1;
            }
        }
        if (_cancelled.load() == true)
        {
            return false;
        }
    }

    // step 3: compute tfirdfs
    std::map<uint64_t, std::map<std::string, double>> tfirdfs;
    double logNumWorks = std::log(_numWorks);
    std::queue<uint64_t> q;
    std::mutex mq;
    for (auto idToTF = termFreqs.begin(); idToTF != termFreqs.end(); idToTF++)
    {
        q.push(idToTF->first);
    }
    int nThreads = std::thread::hardware_concurrency();
    std::thread *threads[nThreads];
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread([&q, &mq, &termFreqs, &tfirdfs, logNumWorks, this]
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
                    auto idToTF = termFreqs.find(id);
                    if (idToTF == termFreqs.end())
                        return;

                    std::map<std::string, double> myTfirdfs;
                    for (auto termToFreq = idToTF->second.begin(); termToFreq != idToTF->second.end(); termToFreq++)
                    {
                        auto termToDf = _dfs.find(termToFreq->first);
                        double irdf = logNumWorks - std::log(termToDf->second);
                        double tfirdf = termToFreq->second.second * irdf;
                        myTfirdfs[termToFreq->first] = tfirdf;
                    }
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        tfirdfs[id] = myTfirdfs;
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

    if (_cancelled.load() == true || tfirdfs.size() < termFreqs.size())
        return false;

    // step 4: save results
    save(y, tfirdfs);
    return true;
}

std::map<uint64_t, std::map<uint64_t, std::vector<double>>> TermTfIrdf::getCitationHighlights(const std::string keywords,
        const std::map<uint64_t,std::vector<uint64_t>> &citations, const std::vector<std::vector<std::string>> &highlightKWs)
{
    // step 1: get the set of citation ids and the set of highlight kws
    std::map<uint64_t, std::map<uint64_t, std::vector<double>>> highlights;
    std::vector<std::set<std::string>> hkws;
    for (size_t i1 = 0; i1 < highlightKWs.size(); i1++)
    {
        std::set<std::string> myHkws;
        for (size_t i2 = 0; i2 < highlightKWs[i1].size(); i2++)
        {
            myHkws.insert(highlightKWs[i1][i2]);
        }
        hkws.push_back(myHkws);
    }
    if (hkws.size() == 0)
        return highlights;
    if (cancelled())
        return highlights;

    std::set<uint64_t> cids;
    for (auto &idToCids: citations)
    {
        cids.insert(idToCids.second.begin(), idToCids.second.end());
    }
    if (cancelled())
        return highlights;

    // step 2: get these cid's term tfirdfs
    std::map<uint64_t, std::map<std::string, double>> tfirdfs;
    if (!load(keywords, cids, &tfirdfs))
        return highlights;
    if (cancelled())
        return highlights;

    // step 3: get highlight weights of these citations
    std::map<uint64_t, std::vector<double>> cHighlights;
    std::queue<uint64_t> q;
    std::mutex mq;
    for (auto iter = tfirdfs.begin(); iter != tfirdfs.end(); iter++)
    {
        q.push(iter->first);
    }
    int nThreads = std::thread::hardware_concurrency();
    std::thread *threads[nThreads];
    std::vector<double> zeros(hkws.size());
    for (size_t i = 0; i < hkws.size(); i++)
    {
        zeros[i] = 0.0;
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread([&q, &mq, &hkws, &tfirdfs, &cHighlights,&zeros]
            {
                std::map<uint64_t, std::vector<double>> myCHighlights;
                const double sqrt2 = sqrt(2);
                for (;;)
                {
                    // get id and my tfirdfs
                    std::map<std::string, double> myTfirdfs;
                    uint64_t id = 0;
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        if (!q.empty())
                        {
                            id = q.front();
                            q.pop();
                            myTfirdfs = tfirdfs[id];
                        }
                        else
                            break;
                    }
                    if (cancelled())
                        return;

                    if (myTfirdfs.size() == 0)
                    {
                        myCHighlights[id] = zeros;
                        continue;
                    }

                    // compute mean and variance of tfirdfs
                    double sum1 = 0, sum2 = 0;
                    for (auto termToW = myTfirdfs.begin(); termToW != myTfirdfs.end(); termToW++)
                    {
                        sum1 += termToW->second;
                        sum2 += termToW->second * termToW->second;
                    }
                    double mean = sum1 / myTfirdfs.size();
                    double varSqr = sum2 / myTfirdfs.size() - mean * mean;
                    double var = varSqr < 0 ? 0 : sqrt(varSqr);
                    if (cancelled())
                        return;

                    // compute the sum of normalized weights and the sum of highlight weights
                    double maxNorm = 1e-6;
                    std::vector<double> maxHigh(hkws.size());
                    for (size_t i = 0; i < hkws.size(); i++)
                    {
                        maxHigh[i] = 0;
                    }
                    for (auto termToW = myTfirdfs.begin(); termToW != myTfirdfs.end(); termToW++)
                    {
                        double w = 0.5 + 0.5 * erf((termToW->second - mean) / (sqrt2 * var));
                        if (w > maxNorm)
                            maxNorm = w;
                        for (size_t i = 0; i < hkws.size(); i++)
                        {
                            if (hkws[i].find(termToW->first) != hkws[i].end() && w > maxHigh[i])
                                maxHigh[i] = w;
                        }

                    }
                    if (cancelled())
                        return;

                    for (size_t i = 0; i < maxHigh.size(); i++)
                    {
                        maxHigh[i] /= maxNorm;
                    }
                    myCHighlights[id] = maxHigh;

                    if (_cancelled.load() == true)
                    {
                        return;
                    }
                }
                if (myCHighlights.size() > 0)
                {
                    std::lock_guard<std::mutex> lock(mq);
                    cHighlights.insert(myCHighlights.begin(), myCHighlights.end());
                }
            });
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid]->join();
        delete threads[tid];
    }
    if (cancelled())
        return highlights;

    // step 4: assemble highlights from cHighlights
    for (auto &idToCids: citations)
    {
        std::map<uint64_t, std::vector<double>> myH;
        for (auto cid: idToCids.second)
        {
            auto myCH = cHighlights.find(cid);
            if (myCH != cHighlights.end())
                myH[cid] = myCH->second;
            else
                myH[cid] = zeros;
        }
        highlights[idToCids.first] = myH;
        if (cancelled())
            return highlights;
    }
    return highlights;
}

bool TermTfIrdf::removeOneYear(const std::string keywords, int y)
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

    {
        std::stringstream ss;
        ss << "DELETE FROM pub_scope_tfirdfs WHERE year = " << y << " AND scope_keywords = '" << keywords << "';";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(1, strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
        }
    }

    {
        std::stringstream ss;
        ss << "DELETE FROM scope_dfs WHERE year = " << y << " AND keywords = '" << keywords << "';";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(1, strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
        }
    }

    sqlite3_close(db);
    return rc == SQLITE_OK;
}
