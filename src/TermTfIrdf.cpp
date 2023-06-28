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

    if (tfirdfs.size() < termFreqs.size())
        return false;

    // step 4: save results
    save(y, tfirdfs);
    return true;
}
