#include "BitermDf.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <CallbackData.h>
#include <wxFFileLog.h>
#include <StringProcessing.h>
#include <cstdlib>
#include <sstream>
#include <set>
#include <queue>
#include <thread>
#include <mutex>

BitermDf::BitermDf(const std::string path, const std::string kws, TermTfIrdf *tt) : _scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
    _tt = tt;
}

BitermDf::~BitermDf()
{
    //dtor
}

bool BitermDf::finished()
{
    int n = numSteps();
    for (int i = 0; i < n; i++)
    {
        if (!load(_y0 + i, NULL))
            return false;
    }
    return true;
}

const char *BitermDf::name()
{
    return "Biterm Document Frequencies Counting.";
}

int BitermDf::numSteps()
{
    return _y2 - _y0;
}

void BitermDf::doStep(int stepId)
{
    process(_y0 + stepId);
}

void getBitermDfs(std::string strBdfs, std::map<std::string, int> *bitermDfs)
{
    std::vector<std::string> bdfStrs = splitString(strBdfs, ",");
    for (std::string bdfStr: bdfStrs)
    {
        std::vector<std::string> bfStrs = splitString(bdfStr, ":");
        std::string biterm = bfStrs[0];
        int freq = atoi(bfStrs[1].c_str());
        (*bitermDfs)[biterm] = freq;
    }
}

bool BitermDf::load(const std::string keywords, int y, std::map<std::string, int> *bitermDfs)
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

    // step 1: load scope bitermDfs
    if (bitermDfs != NULL)
    {
        bitermDfs->clear();
        {
            std::stringstream ss;
            ss << "SELECT keywords, year, bdfs FROM scope_bdfs WHERE keywords = '"
                << keywords << "' AND year = " << y << ";";
            logDebug(ss.str().c_str());
            rc = sqlite3_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
            if (rc != SQLITE_OK)
            {
                logDebug(errorMessage);
                sqlite3_close(db);
                return false;
            }
            if (data.results.size() == 0)
            {
                logDebug("no results found.");
                sqlite3_close(db);
                return false;
            }
            getBitermDfs(data.results[0]["bdfs"], bitermDfs);
        }
    }
    else
    {
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_bdfs WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        if (data.results.size() == 0)
        {
            logDebug("no results found.");
            sqlite3_close(db);
            return false;
        }
    }

    sqlite3_close(db);
    return data.results.size() > 0;
}

bool BitermDf::load(int y, std::map<std::string, int> *bitermDfs)
{
    return load(_scope.getKeywords(), y, bitermDfs);
}

std::string getStrBdfs(const std::map<std::string, int> &bitermDfs)
{
    std::stringstream ss;
    for (auto bToF = bitermDfs.begin(); bToF != bitermDfs.end(); bToF++)
    {
        if (bToF != bitermDfs.begin())
            ss << ",";
        ss << bToF->first << ":" << bToF->second;
    }
    return ss.str();
}

bool BitermDf::save(int y, const std::map<std::string, int> &bitermDfs)
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
        "CREATE TABLE IF NOT EXISTS scope_bdfs("
        "keywords TEXT,"
        "year INTEGER,"
        "bdfs TEXT,"
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

    // step 2: save scope bdf token
    std::string keywords = _scope.getKeywords();
    {
        time_t t;
        time(&t);
        std::stringstream ss;
        std::string strBdfs = getStrBdfs(bitermDfs);
        ss << "INSERT OR IGNORE INTO scope_bdfs(keywords, year, bdfs, update_time) VALUES ('"
            << keywords << "'," << y << ",'" << strBdfs << "'," << (int)t << ");";
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

bool BitermDf::process(int y)
{
    if (load(y, NULL))
        return true;

    // step 1: load publication scope tfirdfs
    std::map<uint64_t, std::map<std::string, double>> tfirdfs;
    if (!_tt->load(y, &tfirdfs, false))
        return false;

    // step 2: compute mean tfirdf in publication
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

    // step 3: count biterm df for those with two terms of tfirdf above mean
    std::map<std::string, int> bdfs;
    for (auto idToWorkTfirdfs = tfirdfs.begin(); idToWorkTfirdfs != tfirdfs.end(); idToWorkTfirdfs++)
    {
        q.push(idToWorkTfirdfs->first);
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread(
            [&q, &mq, &tfirdfs, &meanTfirdfs, this, &bdfs]
            {
                std::map<std::string, int> myBdfs;
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
                    auto idToWorkTfirdfs = tfirdfs.find(id);
                    if (idToWorkTfirdfs == tfirdfs.end())
                        return;
                    double mean = meanTfirdfs[idToWorkTfirdfs->first];

                    for (auto iter1 = idToWorkTfirdfs->second.begin(); iter1 != idToWorkTfirdfs->second.end(); iter1++)
                    {
                        if (iter1->second < mean)
                            continue;
                        for (auto iter2 = idToWorkTfirdfs->second.begin(); iter2 != idToWorkTfirdfs->second.end(); iter2++)
                        {
                            if (iter2->second < mean)
                                continue;
                            if (iter1->first < iter2->first)
                            {
                                std::string biterm = iter1->first + "&" + iter2->first;
                                auto btToDf = myBdfs.find(biterm);
                                if (btToDf == myBdfs.end())
                                {
                                    myBdfs[biterm] = 1;
                                }
                                else
                                {
                                    myBdfs[biterm] = btToDf->second + 1;
                                }
                            }
                        }
                    }
                    if (_cancelled.load() == true)
                    {
                        return;
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(mq);
                    for (auto btToDfSrc = myBdfs.begin(); btToDfSrc != myBdfs.end(); btToDfSrc++)
                    {
                        auto btToDfDst = bdfs.find(btToDfSrc->first);
                        if (btToDfDst == bdfs.end())
                        {
                            bdfs[btToDfSrc->first] = btToDfSrc->second;
                        }
                        else
                        {
                            bdfs[btToDfSrc->first] = btToDfSrc->second + btToDfDst->second;
                        }
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
    if (_cancelled.load() == true)
        return false;

    // step 4: save biterm dfs
    return save(y, bdfs);
}
