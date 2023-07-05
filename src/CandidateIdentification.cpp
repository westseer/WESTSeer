#include "CandidateIdentification.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <CallbackData.h>
#include <wxFFileLog.h>
#include <StringProcessing.h>
#include <ctime>
#include <cstdlib>
#include <sstream>

CandidateIdentification::CandidateIdentification(const std::string path, const std::string kws): _scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
}

CandidateIdentification::~CandidateIdentification()
{
    //dtor
}

bool CandidateIdentification::finished()
{
    if (!load(_y2, NULL))
        return false;
    if (!load(_y2 + 5, NULL))
        return false;
    return true;
}

const char *CandidateIdentification::name()
{
    return "Candidate Identification";
}

int CandidateIdentification::numSteps()
{
    return 2;
}

void CandidateIdentification::doStep(int stepId)
{
    if (stepId == 0)
        process(_y2);
    else
        process(_y2 + 5);
}

bool CandidateIdentification::load(const std::string keywords, int y, std::vector<uint64_t> *candidates)
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

    // step 1: load scope candidates
    if (candidates != NULL)
    {
        candidates->clear();
        std::stringstream ss;
        ss << "SELECT keywords, year, candidates FROM scope_candidates WHERE keywords = '"
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
            std::string strCandidates = data.results[0]["candidates"];
            std::vector<std::string> fields = splitString(strCandidates, ",");
            for (std::string field: fields)
            {
                candidates->push_back(std::stoull(field));
            }
        }
    }
    else
    {
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_candidates WHERE keywords = '"
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

bool CandidateIdentification::load(int y, std::vector<uint64_t> *candidates)
{
    return load(_scope.getKeywords(), y, candidates);
}

bool CandidateIdentification::save(int y, const std::vector<uint64_t> &candidates)
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
        "CREATE TABLE IF NOT EXISTS scope_candidates("
        "keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "candidates TEXT,"
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

    // step 2: save scope candidates
    std::string keywords = _scope.getKeywords();
    time_t t;
    time(&t);
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_candidates(keywords, year, update_time, candidates) VALUES ('"
            << keywords << "'," << y << "," << (int)t << ",'";
        for (auto iter = candidates.begin(); iter != candidates.end(); iter++)
        {
            if (iter != candidates.begin())
                ss << ",";
            ss << *iter;
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

bool CandidateIdentification::process(int y)
{
    std::map<uint64_t, int> refCounts;
    for (int i = 0; i < 10; i++)
    {
        std::map<uint64_t, std::vector<uint64_t>> refIdsOfId;
        if (!_scope.getExistingRefIds(y - 6 - i, refIdsOfId))
        {
            continue;
        }
        if (_cancelled.load() == true)
        {
            return false;
        }

        for (auto iter = refIdsOfId.begin(); iter != refIdsOfId.end(); iter++)
        {
            for (uint64_t refId: iter->second)
            {
                auto refIdToCount = refCounts.find(refId);
                if (refIdToCount == refCounts.end())
                {
                    refCounts[refId] = 1;
                }
                else
                {
                    refCounts[refId] = refIdToCount->second + 1;
                }
            }
        }
        if (_cancelled.load() == true)
        {
            return false;
        }
    }

    std::vector<uint64_t> candidates;
    GeneralConfig config;
    int threshold = config.getCitations();
    for (auto iter = refCounts.begin(); iter != refCounts.end(); iter++)
    {
        if (iter->second >= threshold)
        {
            candidates.push_back(iter->first);
        }
    }
    if (_cancelled.load() == true)
    {
        return false;
    }

    save(y, candidates);
    return true;
}
