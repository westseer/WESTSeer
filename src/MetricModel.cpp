#include "MetricModel.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <CallbackData.h>
#include <wxFFileLog.h>
#include <StringProcessing.h>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <set>

MetricModel::MetricModel(const std::string path, const std::string kws, TimeSeriesExtraction *tse, PredictionModel *pm):_scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
    _tse = tse;
    _pm = pm;
}

MetricModel::~MetricModel()
{
    //dtor
}

bool MetricModel::finished()
{
    return load(_y2, NULL) && load(_y2 + 5, NULL);
}

const char *MetricModel::name()
{
    return "Score evaluations";
}

int MetricModel::numSteps()
{
    return 2;
}

void MetricModel::doStep(int stepId)
{
    if (stepId == 0)
        process(_y2);
    else
        process(_y2 + 5);
}

std::string getScoreStr(const std::map<uint64_t, std::vector<double>> &scores)
{
    std::stringstream ss;
    for (auto idToScore = scores.begin(); idToScore != scores.end(); idToScore++)
    {
        if (idToScore != scores.begin())
            ss << ";";
        ss << idToScore->first << ":" << getVectorStr(idToScore->second);
    }
    return ss.str();
}

std::map<uint64_t, std::vector<double>> getScores(std::string scoreStr)
{
    std::vector<std::string> idScoreStrs = splitString(scoreStr,";");
    std::map<uint64_t, std::vector<double>> scores;
    for (std::string s: idScoreStrs)
    {
        std::vector<std::string> fields = splitString(s,":");
        uint64_t id = std::stoull(fields[0]);
        std::vector<std::string> vStrs = splitString(fields[1],",");
        std::vector<double> vs;
        for (size_t i = 0; i < vStrs.size(); i++)
        {
            vs.push_back(atof(vStrs[i].c_str()));
        }
        scores[id] = vs;
    }
    return scores;
}

bool MetricModel::load(const std::string keywords, int y, std::map<uint64_t, std::vector<double>> *scores)
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

    // step 1: load scope metric
    if (scores != NULL)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year, scores FROM scope_metric WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        *scores = getScores(data.results[0]["scores"]);
    }
    else
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_metric WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            if (rc!=SQLITE_OK)
                logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    sqlite3_close(db);
    return true;
}

bool MetricModel::load(int y, std::map<uint64_t, std::vector<double>> *scores)
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

    // step 1: load scope metric
    std::string keywords = _scope.getKeywords();
    if (scores != NULL)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year, scores FROM scope_metric WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        *scores = getScores(data.results[0]["scores"]);
    }
    else
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_metric WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            if (rc!=SQLITE_OK)
                logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    sqlite3_close(db);
    return true;
}

bool MetricModel::save(int y, const std::map<uint64_t, std::vector<double>> &scores)
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
        "CREATE TABLE IF NOT EXISTS scope_metric("
        "keywords TEXT,"
        "year INTEGER,"
        "scores TEXT,"
        "update_time INTEGER,"
        "PRIMARY KEY(keywords,year));"
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

    // step 2: insert scope metric
    std::string keywords = _scope.getKeywords();
    time_t t;
    time(&t);
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_metric(keywords, year, scores, update_time) VALUES ('"
            << keywords << "'," << y << ",'" << getScoreStr(scores) << "'," << (int)t << ");";
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

void median(const std::vector<int> &src, std::vector<int> &dst, int L)
{
    dst.resize(src.size());
    const int m = 2 * L + 1;
    for (int i = 0; i < (int)src.size(); i++)
    {
        std::vector<int> temp(m);
        for (int j = 0; j < m; j++)
        {
            int k = i + j - L;
            if (k < 0)
                temp[j] = 0;
            else if (k >= (int)src.size())
                temp[j] = src[src.size() - 1];
            else
                temp[j] = src[k];
        }
        std::sort(temp.begin(), temp.end());
        dst[i] = temp[L];
    }
}

bool MetricModel::process(int y)
{
    if (load(y, NULL))
        return true;

    // step 1: load time series
    std::map<uint64_t, TSM> timeSeries;
    if (!_tse->load(y, &timeSeries))
        return false;

    // step 2: load prediction
    std::map<uint64_t, std::vector<double>> prediction;
    if (!_pm->load(y, &prediction))
        return false;
    int cy0 = _y1;
    std::map<uint64_t,Publication> candidates;
    {
        std::vector<uint64_t> cids;
        for (auto &cidToPred: prediction)
        {
            cids.push_back(cidToPred.first);
        }

        std::vector<Publication> temp = _scope.getPublications(cids);
        for (Publication & pub: temp)
        {
            candidates[pub.id()] = pub;
            if (pub.year() < cy0)
            {
                cy0 = pub.year();
            }
        }
    }
    int nCYs = _y1 - cy0;
    std::vector<int> cyCitations(nCYs);
    for (int iCY = 0; iCY < nCYs; iCY++)
    {
        cyCitations[iCY] = 0;
    }

    std::map<uint64_t,int> oldCitations[10], newCitations[5];
    double sumOldPubs = 0;
    {
        // step 3: load publications
        std::map<uint64_t, std::vector<uint64_t>> oldPubs[10], newPubs[5];
        for (int i = 0; i < 15; i++)
        {
            int yi = y - 1 - i;
            if (yi >= WESTSeerApp::year())
                continue;

            std::map<uint64_t, std::vector<uint64_t>> pubs;
            if (!_scope.getExistingRefIds(yi, pubs))
                return false;
            if (i < 5)
            {
                newPubs[4 - i] = pubs;
            }
            else
            {
                oldPubs[9 - (i - 5)] = pubs;
            }
        }

        // step 4: get citations per year of candidates
        for (int i = 0; i <10; i++)
        {
            sumOldPubs += oldPubs[i].size();
            for (auto idToRefIds: oldPubs[i])
            {
                for (uint64_t refId: idToRefIds.second)
                {
                    if (prediction.find(refId) != prediction.end())
                    {
                        int cy = candidates[refId].year();
                        int iCY = cy - cy0;
                        if (iCY < nCYs)
                            cyCitations[iCY]++;
                        auto ridToC = oldCitations[i].find(refId);
                        if (ridToC == oldCitations[i].end())
                        {
                            oldCitations[i][refId] = 1;
                        }
                        else
                        {
                            oldCitations[i][refId] = ridToC->second + 1;
                        }
                    }
                }
            }
        }
        for (int i = 0; i <5; i++)
        {
            for (auto idToRefIds: newPubs[i])
            {
                for (uint64_t refId: idToRefIds.second)
                {
                    if (prediction.find(refId) != prediction.end())
                    {
                        auto ridToC = newCitations[i].find(refId);
                        if (ridToC == newCitations[i].end())
                        {
                            newCitations[i][refId] = 1;
                        }
                        else
                        {
                            newCitations[i][refId] = ridToC->second + 1;
                        }
                    }
                }
            }
        }
    }

    // step 5: compute prediction and verification scores for each candidate
    std::map<uint64_t, std::vector<double>> scores;

    std::vector<int> med1CYCitations;
    median(cyCitations, med1CYCitations, 1);

    std::vector<int> med2CYCitations;
    median(cyCitations, med2CYCitations, 2);

    std::vector<int> dev1CYCitations(nCYs);
    std::vector<int> dev2CYCitations(nCYs);
    for (int iCY = 0; iCY < nCYs; iCY++)
    {
        dev1CYCitations[iCY] = cyCitations[iCY] - med1CYCitations[iCY];
        dev2CYCitations[iCY] = cyCitations[iCY] - med2CYCitations[iCY];
    }

    std::vector<bool> rpys1Peak(nCYs);
    std::vector<bool> rpys2Peak(nCYs);
    for (int iCY = 0; iCY < nCYs; iCY++)
    {
        bool peak1 = true;
        bool peak2 = true;
        if (iCY > 0)
        {
            if (dev1CYCitations[iCY - 1] > dev1CYCitations[iCY])
                peak1 = false;
            if (dev2CYCitations[iCY - 1] > dev2CYCitations[iCY])
                peak2 = false;
        }
        if (iCY < nCYs - 1)
        {
            if (dev1CYCitations[iCY + 1] > dev1CYCitations[iCY])
                peak1 = false;
            if (dev2CYCitations[iCY + 1] > dev2CYCitations[iCY])
                peak2 = false;
        }
        rpys1Peak[iCY] = peak1;
        rpys2Peak[iCY] = peak2;
    }

    int nTop10Thres[10], nTop5Thres[10], nTop1Thres[10];
    for (int i = 0; i < 10; i++)
    {
        std::vector<int> temp;
        for (auto idToC: oldCitations[i])
        {
            temp.push_back(idToC.second);
        }
        std::sort(temp.begin(), temp.end());
        int n = (int)temp.size();
        if (n > 0)
        {
            nTop10Thres[i] = temp[9 * n / 10];
            nTop5Thres[i] = temp[19 * n / 20];
            nTop1Thres[i] = temp[99 * n / 100];
        }
        else
        {
            nTop10Thres[i] = 0;
            nTop5Thres[i] = 0;
            nTop1Thres[i] = 0;
        }
    }

    for (auto idToP: prediction)
    {
        uint64_t id = idToP.first;
        std::vector<int> lts = timeSeries[idToP.first].first.first;
        std::vector<double> rts = idToP.second;

        // get old citations
        int oldC[10], oldCOnTopic[10];
        double sumOld = 0, sumOldOnTopic = 0, maxOldOnTopic = 0;
        for (int i = 0; i < 10; i++)
        {
            auto idToC = oldCitations[i].find(id);
            if (idToC != oldCitations[i].end())
            {
                oldC[i] = idToC->second;
            }
            else
            {
                oldC[i] = 0;
            }

            oldCOnTopic[i] = lts[i];
            sumOld += oldC[i];
            sumOldOnTopic += oldCOnTopic[i];
            if (oldCOnTopic[i] > maxOldOnTopic)
                maxOldOnTopic = oldCOnTopic[i];
        }
        if (sumOldOnTopic == 0)
            sumOldOnTopic = 1;

        // get new citations
        int newC[5];
        double newCOnTopic[5];
        double sumNew = 0, sumNewOnTopic = 0;
        for (int i = 0; i < 5; i++)
        {
            auto idToC = newCitations[i].find(id);
            if (idToC != newCitations[i].end())
            {
                newC[i] = idToC->second;
            }
            else
            {
                newC[i] = 0;
            }

            newCOnTopic[i] = rts[i];
            sumNew += newC[i];
            sumNewOnTopic += newCOnTopic[i];
        }

        // compute irdf
        double irdf = log(sumOldPubs / sumOld);
        double predIrdf = log(sumOldPubs / sumOldOnTopic);

        // compute growth
        double meanOld = sumOld * 0.1;
        double meanOldOnTopic = sumOldOnTopic * 0.1;
        double meanNew = sumNew * 0.2;
        double meanNewOnTopic = sumNewOnTopic * 0.2;
        if (meanNewOnTopic < 0)
            meanNewOnTopic = 0;
        double growth = meanNew / meanOld;
        double predGrowth = meanNewOnTopic / meanOldOnTopic;

        // compute scores
        double predScore = predGrowth * predIrdf;
        double realScore = growth * irdf;
        double rpys1C = 0, rpys2C = 0, rpys1NTop10 = 0, rpys2NTop10 = 0, rpys1NTop5 = 0, rpys2NTop5 = 0, rpys1NTop1 = 0, rpys2NTop1 = 0;

        int cy = candidates[id].year();
        int iCY = cy - cy0;
        if (iCY < nCYs)
        {
            if (rpys1Peak[iCY])
            {
                rpys1C = sumOld;
                for (int i = 0; i < 10; i++)
                {
                    auto idToC = oldCitations[i].find(id);
                    if (idToC != oldCitations[i].end())
                    {
                        int c = idToC->second;
                        if (c >= nTop10Thres[i])
                            rpys1NTop10 += 1.0;
                        if (c >= nTop5Thres[i])
                            rpys1NTop5 += 1.0;
                        if (c >= nTop1Thres[i])
                            rpys1NTop1 += 1.0;
                    }
                }
            }

            if (rpys2Peak[iCY])
            {
                rpys2C = sumOld;
                for (int i = 0; i < 10; i++)
                {
                    auto idToC = oldCitations[i].find(id);
                    if (idToC != oldCitations[i].end())
                    {
                        int c = idToC->second;
                        if (c >= nTop10Thres[i])
                            rpys2NTop10 += 1.0;
                        if (c >= nTop5Thres[i])
                            rpys2NTop5 += 1.0;
                        if (c >= nTop1Thres[i])
                            rpys2NTop1 += 1.0;
                    }
                }
            }
        }

        std::vector<double> myScores;
        myScores.push_back(predScore);
        myScores.push_back(realScore);
        for (int i = 0; i <10; i++)
        {
            myScores.push_back(oldCOnTopic[i]);
        }
        for (int i = 0; i < 5; i++)
        {
            myScores.push_back(newCOnTopic[i]);
        }
        for (int i = 0; i <10; i++)
        {
            myScores.push_back(oldC[i]);
        }
        for (int i = 0; i < 5; i++)
        {
            myScores.push_back(newC[i]);
        }
        myScores.push_back(rpys1C);
        myScores.push_back(rpys1NTop10);
        myScores.push_back(rpys1NTop5);
        myScores.push_back(rpys1NTop1);
        myScores.push_back(rpys2C);
        myScores.push_back(rpys2NTop10);
        myScores.push_back(rpys2NTop5);
        myScores.push_back(rpys2NTop1);
        scores[id] = myScores;
    }
    return save(y, scores);
}

std::vector<std::string> MetricModel::getScopesWithMetrics(const std::string path)
{
    std::vector<std::string> scopes = ResearchScope::getResearchScopes(path);
    if (scopes.size() == 0)
        return scopes;

    std::vector<std::string> evaluatedScopes;
    sqlite3 *db = NULL;
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + path));
        return evaluatedScopes;
    }
    char *errorMessage = NULL;

    // step 1: load scope metric
    int y = WESTSeerApp::year();
    for (std::string keywords: scopes)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_metric WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            continue;
        }
        if (data.results.size() > 0)
        {
            evaluatedScopes.push_back(keywords);
        }
    }

    sqlite3_close(db);
    return evaluatedScopes;
}
