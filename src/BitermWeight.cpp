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

bool BitermWeight::load(const std::string keywords, const std::set<uint64_t>& ids, std::map<uint64_t, std::map<std::string, double>> *bitermWeights)
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

    // step 1: load scope bitermWeights
    if (bitermWeights != NULL)
    {
        bitermWeights->clear();
        std::vector<uint64_t> idList(ids.begin(),ids.end());
        for (size_t i0 = 0; i0 < idList.size(); i0 += 1000)
        {
            CallbackData data;
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, biterm_weights FROM pub_scope_bws WHERE scope_keywords = '"
                << keywords << "' AND id in (";
            size_t i1 = i0 + 1000;
            if (i1 > idList.size())
                i1 = idList.size();
            for (size_t i = i0; i < i1; i++)
            {
                if (i != i0)
                    ss << ",";
                ss << idList[i];
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
            if (cancelled())
                break;
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
            if (cancelled())
                break;
        }
    }

    sqlite3_close(db);
    return !cancelled();
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
            CallbackData::updateWriteCount(i1 - i0, strSql.size());
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

bool BitermWeight::process(int y)
{
    if (load(y, NULL))
        return true;

    // step 1: load tfirdfs
    std::map<uint64_t, std::map<std::string, double>> tfirdfs;
    if (!_tt->load(y, &tfirdfs, false))
        return false;
    if (cancelled())
        return false;

    // step 2: load bitermDfs
    std::map<std::string, int> bdfs[10];
    int numPubs = 0;
    for (int i = 0; i < 10; i++)
    {
        _bdf->load(y - i, &(bdfs[9 - i]));
        if (cancelled())
            return false;
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
    if (cancelled())
        return false;
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
                    if (cancelled())
                        return;
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

    if (cancelled() || meanTfirdfs.size() < tfirdfs.size())
        return false;

    // step 4: compute biterm weights
    std::map<uint64_t, std::map<std::string, double>> bitermWeights;
    for (auto idToWorkTfirdfs = tfirdfs.begin(); idToWorkTfirdfs != tfirdfs.end(); idToWorkTfirdfs++)
    {
        q.push(idToWorkTfirdfs->first);
    }
    if (cancelled())
        return false;
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
                    if (cancelled())
                        return;

                    auto idToTTf = tfirdfs.find(id);
                    if (idToTTf == tfirdfs.end())
                        return;
                    double mean = meanTfirdfs[id];

                    std::map<std::string, double> bwOfId;
                    for (auto tToTf1 = idToTTf->second.begin(); tToTf1 != idToTTf->second.end(); tToTf1++)
                    {
                        if (tToTf1->second < mean)
                            continue;
                        if (cancelled())
                            return;
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
    if (cancelled() || bitermWeights.size() < tfirdfs.size())
        return false;


    save(y, bitermWeights);
    return true;
}

std::map<uint64_t, std::map<uint64_t, std::vector<double>>> BitermWeight::getCitationHighlights(const std::string keywords,
        const std::map<uint64_t,std::vector<uint64_t>> &citations, const std::vector<std::vector<std::string>> &highlightKWs)
{
    // step 1: get the set of citation ids and the set of highlight kws
    std::map<uint64_t, std::map<uint64_t, std::vector<double>>> highlights;
    std::set<uint64_t> cids;
    std::vector<std::set<std::string>> hkws;
    for (size_t i1 = 0; i1 < highlightKWs.size(); i1++)
    {
        for (size_t i3 = i1+1; i3 < highlightKWs.size(); i3++)
        {
            std::set<std::string> myHkws;
            for (size_t i2 = 0; i2 < highlightKWs[i1].size(); i2++)
            {
                for (size_t i4 = 0; i4 < highlightKWs[i3].size(); i4++)
                {
                    std::string biterm;
                    if (highlightKWs[i1][i2] < highlightKWs[i3][i4])
                    {
                        biterm = highlightKWs[i1][i2] + "&" + highlightKWs[i3][i4];
                    }
                    else
                    {
                        biterm = highlightKWs[i3][i4] + "&" + highlightKWs[i1][i2];
                    }
                    myHkws.insert(biterm);
                }
            }
            hkws.push_back(myHkws);
        }
    }
    if (hkws.size() == 0)
        return highlights;
    if (cancelled())
        return highlights;
    for (auto &idToCids: citations)
    {
        cids.insert(idToCids.second.begin(), idToCids.second.end());
        if (cancelled())
            return highlights;
    }

    // step 2: get these cid's biterm weights
    std::map<uint64_t, std::map<std::string, double>> bws;
    if (!load(keywords, cids, &bws))
        return highlights;
    if (cancelled())
        return highlights;

    // step 3: get highlight weights of these citations
    std::map<uint64_t, std::vector<double>> cHighlights;
    std::queue<uint64_t> q;
    std::mutex mq;
    for (auto iter = bws.begin(); iter != bws.end(); iter++)
    {
        q.push(iter->first);
    }
    if (cancelled())
        return highlights;
    int nThreads = std::thread::hardware_concurrency();
    std::thread *threads[nThreads];
    std::vector<double> zeros(hkws.size());
    for (size_t i = 0; i < hkws.size(); i++)
    {
        zeros[i] = 0.0;
    }
    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread([&q, &mq, &hkws, &bws, &cHighlights,&zeros]
            {
                std::map<uint64_t, std::vector<double>> myCHighlights;
                const double sqrt2 = sqrt(2);
                for (;;)
                {
                    // get id and my tfirdfs
                    std::map<std::string, double> myBws;
                    uint64_t id = 0;
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        if (!q.empty())
                        {
                            id = q.front();
                            q.pop();
                            myBws = bws[id];
                        }
                        else
                            break;
                    }
                    if (cancelled())
                        return;

                    if (myBws.size() == 0)
                    {
                        myCHighlights[id] = zeros;
                        continue;
                    }

                    // compute mean and variance of tfirdfs
                    double sum1 = 0, sum2 = 0;
                    for (auto bToW = myBws.begin(); bToW != myBws.end(); bToW++)
                    {
                        sum1 += bToW->second;
                        sum2 += bToW->second * bToW->second;
                    }
                    double mean = sum1 / myBws.size();
                    double varSqr = sum2 / myBws.size() - mean * mean;
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
                    for (auto bToW = myBws.begin(); bToW != myBws.end(); bToW++)
                    {
                        double w = 0.5 + 0.5 * erf((bToW->second - mean) / (sqrt2 * var));
                        if (w > maxNorm)
                            maxNorm = w;
                        for (size_t i = 0; i < hkws.size(); i++)
                        {
                            if (hkws[i].find(bToW->first) != hkws[i].end() && w > maxHigh[i])
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

bool BitermWeight::removeOneYear(const std::string keywords, int y)
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
        ss << "DELETE FROM pub_scope_bws WHERE year = " << y << " AND scope_keywords = '" << keywords << "';";
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
        ss << "DELETE FROM scope_bw_tokens WHERE year = " << y << " AND keywords = '" << keywords << "';";
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
