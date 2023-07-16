#include "ResearchScope.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <StringProcessing.h>
#include <sqlite3.h>
#include <time.h>
#include <CallbackData.h>
#include <wxFFileLog.h>
#include <BitermWeight.h>
#include <TopicIdentification.h>
#include <GeneralConfig.h>

std::vector<std::string> ResearchScope::getResearchScopes(const std::string path)
{
    std::vector<std::string> results;
    sqlite3 *db = NULL;
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + path));
        return results;
    }

    const char *sql = "SELECT keywords FROM research_scopes ORDER BY update_time ASC;";
    CallbackData data;
    char *errorMessage = NULL;
    logDebug(sql);
    rc = sqlite3x_exec(db, sql, CallbackData::sqliteCallback, &data, &errorMessage);
    if (rc == SQLITE_OK)
    {
        for (auto &result: data.results)
        {
            results.push_back(result["keywords"]);
        }
    }
    else
    {
        logDebug(errorMessage);
    }
    sqlite3_close(db);
    return results;
}

ResearchScope::ResearchScope(const std::string path, const std::string keywords)
{
    //ctor
    _path = path;
    std::vector<std::string> kwss = splitString(keywords,";");
    for (std::string kwsStr: kwss)
    {
        std::vector<std::string> kws = splitString(kwsStr,",");
        std::sort(kws.begin(), kws.end());
        _kwss.push_back(kws);
    }
}

ResearchScope::~ResearchScope()
{
    //dtor
}

Publication ResearchScope::getPublication(uint64_t id)
{
    Publication noPub;
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return noPub;
    }
    char *errorMessage = NULL;

    std::stringstream ss;
    ss << "SELECT id, year, title, abstract, source, language, authors, ref_ids FROM publications WHERE id = " << id << ";";
    std::string sqlStr = ss.str();
    logDebug(sqlStr.c_str());
    CallbackData data;
    rc = sqlite3x_exec(db, sqlStr.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
    if (rc != SQLITE_OK)
    {
        logError(errorMessage);
        sqlite3_close(db);
        return noPub;
    }
    if (data.results.size() == 0)
    {
        logError("Publication not found.");
        sqlite3_close(db);
        return noPub;
    }
    Publication temp(data.results[0]);
    sqlite3_close(db);
    return temp;
}

std::vector<Publication> ResearchScope::getPublications(std::vector<uint64_t> ids)
{
    std::vector<Publication> pubs;
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return pubs;
    }
    char *errorMessage = NULL;

    std::stringstream ss;
    ss << "SELECT id, year, title, abstract, source, language, authors, ref_ids FROM publications WHERE id IN (";
    for (size_t i = 0; i < ids.size(); i++)
    {
        if (i > 0)
            ss << ",";
        ss << ids[i];
    }
    ss << ");";
    std::string sqlStr = ss.str();
    logDebug(sqlStr.c_str());
    CallbackData data;
    rc = sqlite3x_exec(db, sqlStr.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
    if (rc != SQLITE_OK)
    {
        logError(errorMessage);
        sqlite3_close(db);
        return pubs;
    }
    for (auto &result: data.results)
    {
        Publication temp(result);
        pubs.push_back(temp);
    }
    sqlite3_close(db);
    return pubs;
}

std::vector<Publication> ResearchScope::getReferences(uint64_t id)
{
    Publication me = getPublication(id);
    return getPublications(me.refIds());
}

std::vector<Publication> ResearchScope::getCitations(uint64_t id, int ye)
{
    std::vector<Publication> myCitations;
    for (int y = ye - 15; y < ye; y++)
    {
        std::map<uint64_t, std::vector<uint64_t>> refIdsOfId;
        std::vector<uint64_t> myCitIds;
        if (getExistingRefIds(y, refIdsOfId))
        {
            for (auto idToRefIds: refIdsOfId)
            {
                for (uint64_t refId: idToRefIds.second)
                {
                    if (refId == id)
                    {
                        myCitIds.push_back(idToRefIds.first);
                        break;
                    }
                }
            }
            if (myCitIds.size() > 0)
            {
                std::vector<Publication> temp = getPublications(myCitIds);
                for (Publication p: temp)
                {
                    myCitations.push_back(p);
                }
            }
        }
    }
    return myCitations;
}

std::map<uint64_t,std::vector<Publication>> ResearchScope::getCitations(const std::vector<uint64_t> &ids, int ye)
{
    std::map<uint64_t,std::vector<uint64_t>> cids;
    for (uint64_t id: ids)
    {
        std::vector<uint64_t> myCids;
        cids[id] = myCids;
    }
    for (int y = ye - 15; y < ye; y++)
    {
        std::map<uint64_t, std::vector<uint64_t>> refIdsOfId;
        std::vector<uint64_t> myCitIds;
        if (getExistingRefIds(y, refIdsOfId))
        {
            for (auto idToRefIds: refIdsOfId)
            {
                for (uint64_t refId: idToRefIds.second)
                {
                    auto refIdToCids = cids.find(refId);
                    if (refIdToCids != cids.end())
                    {
                        refIdToCids->second.push_back(idToRefIds.first);
                    }
                }
            }
        }
    }
    std::set<uint64_t> allCids;
    for (auto &idToCids: cids)
    {
        allCids.insert(idToCids.second.begin(), idToCids.second.end());
    }

    std::map<uint64_t, Publication> cpubs;
    {
        std::vector<uint64_t> temp(allCids.begin(), allCids.end());
        std::vector<Publication> temp2 = getPublications(temp);
        for (Publication & pub: temp2)
        {
            cpubs[pub.id()] = pub;
        }
    }

    std::map<uint64_t,std::vector<Publication>> citations;
    for (auto &idToCids: cids)
    {
        uint64_t id = idToCids.first;
        std::vector<Publication> myCitations;
        for (uint64_t cid: idToCids.second)
        {
            myCitations.push_back(cpubs[cid]);
        }
        citations[id] = myCitations;
    }
    return citations;
}

std::pair<std::string,std::string> ResearchScope::getTopic(uint64_t id, int ye, TopicIdentification *ti)
{
    std::map<uint64_t,std::pair<std::string,std::string>> topics;
    std::pair<std::string,std::string> noTopic("","");
    if (!ti->load(ye, &topics))
        return noTopic;
    auto idToTopic = topics.find(id);
    if (idToTopic == topics.end())
        return noTopic;
    return idToTopic->second;
}

std::vector<Publication> ResearchScope::getTopicPublications(uint64_t id, int ye, BitermWeight *bw, TopicIdentification *ti)
{
    std::vector<Publication> noPubs;
    std::pair<std::string,std::string> myTopic = getTopic(id, ye, ti);
    if (myTopic.first == "")
        return noPubs;

    std::vector<std::string> biterms = splitString(myTopic.first,"|");
    std::set<std::string> bitermSet(biterms.begin(),biterms.end());

    std::set<uint64_t> myTidSet;
    for (int y = ye - 15; y < ye; y++)
    {
        std::map<uint64_t, std::map<std::string, double>> bws;
        if (bw->load(y, &bws))
        {
            for (auto cidToBW: bws)
            {
                uint64_t cid = cidToBW.first;
                for (auto bToW: cidToBW.second)
                {
                    if (bitermSet.find(bToW.first) != bitermSet.end())
                    {
                        myTidSet.insert(cid);
                        break;
                    }
                }
            }
        }
    }

    std::vector<uint64_t> myTids(myTidSet.begin(), myTidSet.end());
    return getPublications(myTids);
}

bool ResearchScope::storable()
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    char *errorMessage = NULL;
    const char*sqls[] =
    {
        "CREATE TABLE IF NOT EXISTS publications("
        "id INTEGER PRIMARY KEY ASC,"
        "year INTEGER,"
        "title TEXT,"
        "abstract TEXT,"
        "source TEXT,"
        "language TEXT,"
        "authors TEXT,"
        "ref_ids TEXT);",

        "CREATE TABLE IF NOT EXISTS openalex_queries("
        "combination TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "ids TEXT,"
        "ref_ids TEXT,"
        "PRIMARY KEY(combination,year));",

        "CREATE TABLE IF NOT EXISTS openalex_tokens("
        "combination TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "PRIMARY KEY(combination,year));",

        "CREATE TABLE IF NOT EXISTS research_scopes("
        "keywords TEXT PRIMARY KEY,"
        "combinations TEXT,"
        "update_time INTEGER);",
    };
    for (const char*sql: sqls)
    {
        logDebug(sql);
        rc = sqlite3x_exec(db, sql, NULL, NULL, &errorMessage);
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

std::string ResearchScope::getKeywords() const
{
    std::stringstream ss;
    for (size_t i1 = 0; i1 <_kwss.size(); i1++)
    {
        if (i1 >0)
            ss << ";";
        for (size_t i2 = 0; i2 < _kwss[i1].size(); i2++)
        {
            if (i2 > 0)
                ss << ",";
            ss << _kwss[i1][i2];
        }
    }
    return ss.str();
}

std::string ResearchScope::getCombinations()
{
    std::stringstream ss;
    int numCombs = numCombinations();
    for (int i = 0; i < numCombs; i++)
    {
        if (i > 0)
            ss << ",";
        ss << getCombination(i);
    }

    return ss.str();
}

int ResearchScope::numCombinations() const
{
    size_t numCombs = 1;
    for (size_t i1 = 0; i1 <_kwss.size(); i1++)
    {
        numCombs *= _kwss[i1].size();
    }
    return (int) numCombs;
}

std::string ResearchScope::getCombination(int i) const
{
    int temp = i % numCombinations();
    std::vector<std::string> comb;
    for (size_t i1 = 0; i1 < _kwss.size(); i1++)
    {
        int s1 = (int)_kwss[i1].size();
        int i2 = temp % s1;
        temp /= s1;
        comb.push_back(_kwss[i1][i2]);
    }
    std::sort(comb.begin(), comb.end());
    std::stringstream ss;
    for (size_t i = 0; i < comb.size(); i++)
    {
        if (i > 0)
            ss << "&";
        ss << comb[i];
    }
    return ss.str();
}

int ResearchScope::numPublications(const int y) const
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;

    std::set<uint64_t> ids;
    int numCombs = numCombinations();
    for (int idxComb = 0; idxComb < numCombs; idxComb++)
    {
        CallbackData data;
        char *errorMessage = NULL;
        std::stringstream ss;
        ss << "SELECT combination, year, ids FROM openalex_queries"
           " WHERE combination = '" << getCombination(idxComb) << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (data.results.size() > 0)
        {
            std::vector<std::string> idStrs = splitString(data.results[0]["ids"], ",");
            for (std::string idStr: idStrs)
            {
                ids.insert(std::stoull(idStr));
            }
        }
    }

    sqlite3_close(db);
    return (int)ids.size();
}

bool ResearchScope::init()
{
    if (!storable())
        return false;

    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    CallbackData data;
    char *errorMessage = NULL;

    std::string keywords = getKeywords();
    std::string combinations = getCombinations();
    time_t t;
    time(&t);
    std::stringstream ss;
    ss << "INSERT OR IGNORE INTO research_scopes(keywords,combinations,update_time) VALUES('"
       << keywords << "','" << combinations << "'," << (int)t
       << ");";
    std::string sql = ss.str();
    CallbackData::updateWriteCount(1, sql.size());
    logDebug(sql.c_str());
    rc = sqlite3x_exec(db, sql.c_str(), NULL, NULL, &errorMessage);
    sqlite3_close(db);
    if (rc != SQLITE_OK)
    {
        logError(errorMessage);
    }
    return rc == SQLITE_OK;
}

bool ResearchScope::load(const int y, std::map<uint64_t, Publication> &pubsOfY, std::map<uint64_t, Publication> &refsOfY)
{
    pubsOfY.clear();
    refsOfY.clear();
    int nCombs = numCombinations();

    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;
    char *errorMessage = NULL;

    std::set<uint64_t> ids;
    for (int iComb = 0; iComb < nCombs; iComb++)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT combination, year, ids FROM openalex_queries"
           " WHERE combination = '" << getCombination(iComb) << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }

        std::string strIds = data.results[0]["ids"];
        std::vector<std::string> idStrs = splitString(strIds, ",");
        for (std::string idStr: idStrs)
        {
            ids.insert(std::stoull(idStr));
        }
    }


    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT id, year, title, abstract, source, language, authors, ref_ids FROM publications WHERE id in (";
        for (auto iter = ids.begin(); iter != ids.end(); iter++)
        {
            if (iter != ids.begin())
                ss << ",";
            ss << *iter;
        }
        ss << ");";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }

        for (auto &result: data.results)
        {
            Publication pub(result);
            pubsOfY[pub.id()] = pub;
        }
    }

    // todo: load references
    std::set<uint64_t> refIds;
    for (auto idToPub: pubsOfY)
    {
        for (uint64_t refId: idToPub.second.refIds())
        {
            refIds.insert(refId);
        }
    }
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT id, year, title, abstract, source, language, authors, ref_ids FROM publications WHERE id in (";
        for (auto iter = refIds.begin(); iter != refIds.end(); iter++)
        {
            if (iter != refIds.begin())
                ss << ",";
            ss << *iter;
        }
        ss << ");";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }

        for (auto &result: data.results)
        {
            Publication pub(result);
            refsOfY[pub.id()] = pub;
        }
    }

    sqlite3_close(db);
    return true;
}

bool ResearchScope::load(int idxComb, const int y, std::map<uint64_t, Publication> &pubsOfY)
{
    pubsOfY.clear();
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;
    CallbackData data;
    char *errorMessage = NULL;

    {
        std::stringstream ss;
        ss << "SELECT combination, year, ids FROM openalex_queries"
           " WHERE combination = '" << getCombination(idxComb) << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }
    }


    {
        std::stringstream ss;
        std::string strIds = data.results[0]["ids"];
        data.results.clear();
        ss << "SELECT id, year, title, abstract, source, language, authors, ref_ids FROM publications WHERE id in ("
           << strIds << ");";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }


    for (auto &result: data.results)
    {
        Publication pub(result);
        pubsOfY[pub.id()] = pub;
    }
    sqlite3_close(db);
    return true;
}

bool ResearchScope::load(int idxComb, const int y)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;
    CallbackData data;
    char *errorMessage = NULL;

    std::stringstream ss;
    ss << "SELECT combination, year FROM openalex_tokens"
       " WHERE combination = '" << getCombination(idxComb) << "' AND year = " << y << ";";
    logDebug(ss.str().c_str());
    rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
    if (rc != SQLITE_OK)
    {
        logDebug(errorMessage);
    }
    if (rc != SQLITE_OK || data.results.size() == 0)
    {
        sqlite3_close(db);
        return false;
    }
    sqlite3_close(db);
    return true;
}

bool ResearchScope::removeOneYear(const int y)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;
    CallbackData data;
    char *errorMessage = NULL;

    // remove tokens
    int nCombs = numCombinations();
    int rc1, rc2;
    {
        std::stringstream ss;
        ss << "DELETE FROM openalex_tokens WHERE year = " << y << " AND combination IN (";
        for (int idxComb = 0; idxComb < nCombs; idxComb++)
        {
            if (idxComb > 0)
                ss << ",";
            ss << "'" << getCombination(idxComb) << "'";
        }
        ss << ");";

        std::string strSql = ss.str();
        CallbackData::updateWriteCount(nCombs, strSql.size());
        logDebug(strSql.c_str());
        rc1 = sqlite3x_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc1 != SQLITE_OK)
        {
            logError(errorMessage);
        }
    }

    // remove queries
    {
        std::stringstream ss;
        ss << "DELETE FROM openalex_queries WHERE year = " << y << " AND combination IN (";
        for (int idxComb = 0; idxComb < nCombs; idxComb++)
        {
            if (idxComb > 0)
                ss << ",";
            ss << "'" << getCombination(idxComb) << "'";
        }
        ss << ");";

        std::string strSql = ss.str();
        CallbackData::updateWriteCount(nCombs, strSql.size());
        logDebug(strSql.c_str());
        rc2 = sqlite3x_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc2 != SQLITE_OK)
        {
            logError(errorMessage);
        }
    }

    sqlite3_close(db);
    return rc1 == SQLITE_OK && rc2 == SQLITE_OK;
}

int ResearchScope::getLatestYear(int yb, int ye)
{
    int nY = ye - yb;
    int nCombs = numCombinations();
    int n = nY * nCombs;
    if (n <= 0)
        return 0;

    for (int iY = nY-1; iY >= 0; iY--)
    {
        int y = yb + iY;
        for (int iComb = 0; iComb < nCombs; iComb ++)
        {
            if (load(iComb, y))
                return y;
        }
    }
    return 0;
}

int ResearchScope::getProgress(int yb, int ye)
{
    int nY = ye - yb;
    int nCombs = numCombinations();
    int n = nY * nCombs;
    if (n <= 0)
        return 0;

    int nFinished = 0;
    for (int iY = 0; iY < nY; iY++)
    {
        int y = yb + iY;
        for (int iComb = 0; iComb < nCombs; iComb ++)
        {
            if (load(iComb, y))
                nFinished++;
        }
    }
    return 100 * nFinished / n;
}

bool ResearchScope::save(const std::map<uint64_t, Publication> &pubs)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    CallbackData data;
    char *errorMessage = NULL;

    // ---------------------------------------------------------------------------
    // step 1: find old ids
    {
        std::stringstream ss;
        ss << "SELECT id FROM publications where id in (";
        int iPub = 0;
        for (auto idToPub: pubs)
        {
            if (iPub++ > 0)
                ss << ",";
            ss << idToPub.first;
        }
        ss << ");";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    if (data.results.size() == pubs.size())
    {
        sqlite3_close(db);
        return true;
    }

    std::set<uint64_t> oldIdSet;
    for (auto result: data.results)
    {
        oldIdSet.insert(std::stoull(result["id"]));
    }

    // ---------------------------------------------------------------------------
    // step 2: insert publications with new ids
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO publications(id, year, title, abstract, source, language, authors, ref_ids) VALUES ";
        int iPub = 0;
        for (auto idToPub: pubs)
        {
            if (oldIdSet.find(idToPub.first) != oldIdSet.end())
                continue;

            if (iPub++ > 0)
                ss << ",";

            ss << "("
               << idToPub.second.id() << ","
               << idToPub.second.year() << ",'"
               << idToPub.second.title() << "','"
               << idToPub.second.abstract() << "','"
               << idToPub.second.source() << "','"
               << idToPub.second.language() << "','";

            const std::vector<wxString> &authors = idToPub.second.authors();
            for (size_t i = 0; i < authors.size(); i++)
            {
                if (i > 0)
                    ss << ",";
                ss << authors[i];
            }
            ss << "','";

            std::vector<uint64_t> refIds = idToPub.second.refIds();
            for (size_t i = 0; i < refIds.size(); i++)
            {
                if (i > 0)
                    ss << ",";
                ss << refIds[i];
            }
            ss << "')";
        }
        ss << ";";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(pubs.size(), strSql.size());
        rc = sqlite3x_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
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

bool ResearchScope::save(int idxComb, const int y, const std::map<uint64_t, Publication> &pubsOfY)
{
    save(pubsOfY);

    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    CallbackData data;
    char *errorMessage = NULL;

    // ---------------------------------------------------------------------------
    // step 1: get ref ids
    std::set<uint64_t> refIds;
    for (auto idToPub: pubsOfY)
    {
        vector<uint64_t> refIdsOfPub = idToPub.second.refIds();
        refIds.insert(refIdsOfPub.begin(), refIdsOfPub.end());
    }

    // ---------------------------------------------------------------------------
    // step 2: save ids and ref ids
    std::string combination = getCombination(idxComb);
    time_t t;
    time(&t);
    std::stringstream ss;
    ss << "INSERT OR IGNORE INTO openalex_queries(combination,year,update_time,ids,ref_ids) VALUES ('"
       << combination << "'," << y << "," << (int) t << ",'";
    int iPub = 0;
    for (auto idToPub: pubsOfY)
    {
        if (iPub++ > 0)
            ss << ",";
        ss << idToPub.first;
    }
    ss << "','";
    int iRef = 0;
    for (uint64_t refId: refIds)
    {
        if (iRef++ > 0)
            ss << ",";
        ss << refId;
    }
    ss << "');";
    std::string strSql = ss.str();
    CallbackData::updateWriteCount(1, strSql.size());
    logDebug(strSql.c_str());
    rc = sqlite3x_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
    if (rc != SQLITE_OK)
    {
        logError(errorMessage);
    }
    sqlite3_close(db);
    return rc == SQLITE_OK;
}

bool ResearchScope::save(int idxComb, const int y)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    CallbackData data;
    char *errorMessage = NULL;

    std::string combination = getCombination(idxComb);
    time_t t;
    time(&t);
    std::stringstream ss;
    ss << "INSERT OR IGNORE INTO openalex_tokens(combination,year,update_time) VALUES ('"
       << combination << "'," << y << "," << (int) t << ");";
    std::string strSql = ss.str();
    CallbackData::updateWriteCount(1, strSql.size());
    logDebug(strSql.c_str());
    rc = sqlite3x_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
    if (rc != SQLITE_OK)
    {
        logError(errorMessage);
    }
    sqlite3_close(db);
    return rc == SQLITE_OK;
}

bool ResearchScope::getMissingRefIds(int idxComb, const int y, std::vector<uint64_t> &newRefIds)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    CallbackData data;
    char *errorMessage = NULL;

    // ---------------------------------------------------------------------------
    // step 1: get ref ids of combination and year
    {
        std::stringstream ss;
        ss << "SELECT combination, year, ref_ids FROM openalex_queries"
           " WHERE combination = '" << getCombination(idxComb) << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }
    }
    std::string refIdsStr = data.results[0]["ref_ids"];
    std::vector<std::string> refIDStrs = splitString(refIdsStr, ",");
    std::vector<uint64_t> refIds;
    for (std::string s : refIDStrs)
    {
        refIds.push_back(std::stoull(s));
    }

    // ---------------------------------------------------------------------------
    // step 2: find old ids
    {
        std::stringstream ss;
        data.results.clear();
        ss << "SELECT id FROM publications WHERE id IN (" << refIdsStr << ");";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }
    }
    std::set<uint64_t> oldRefIds;
    for (auto result: data.results)
    {
        oldRefIds.insert(std::stoull(result["id"]));
    }

    // ---------------------------------------------------------------------------
    // step 3: output new ids
    newRefIds.clear();
    for (uint64_t id: refIds)
    {
        if (oldRefIds.find(id) == oldRefIds.end())
        {
            newRefIds.push_back(id);
        }
    }
    sqlite3_close(db);
    return true;
}

bool ResearchScope::getExistingRefIds(const int y, std::map<uint64_t, std::vector<uint64_t>> &refIdsOfId)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(_path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + _path));
        return false;
    }
    char *errorMessage = NULL;

    int numCombs = numCombinations();
    std::set<uint64_t> ids;
    std::set<uint64_t> refIds;

    // ---------------------------------------------------------------------------
    // step 1: get ids and ref ids of combination and year
    for (int idxComb = 0; idxComb <numCombs; idxComb++)
    {
        std::stringstream ss;
        ss << "SELECT combination, year, ids, ref_ids FROM openalex_queries"
           " WHERE combination = '" << getCombination(idxComb) << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        CallbackData data;
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }

        std::string idsStr = data.results[0]["ids"];
        std::vector<std::string> idStrs = splitString(idsStr, ",");
        for (std::string s : idStrs)
        {
            ids.insert(std::stoull(s));
        }

        std::string refIdsStr = data.results[0]["ref_ids"];
        std::vector<std::string> refIDStrs = splitString(refIdsStr, ",");
        for (std::string s : refIDStrs)
        {
            refIds.insert(std::stoull(s));
        }
    }

    // ---------------------------------------------------------------------------
    // step 2: find existing ref ids
    std::set<uint64_t> existingRefIds;
    {
        std::stringstream ss;
        ss << "SELECT id FROM publications WHERE id IN (";
        for (auto iter = refIds.begin(); iter != refIds.end(); iter++)
        {
            if (iter != refIds.begin())
                ss << ",";
            ss << *iter;
        }
        ss << ");";
        logDebug(ss.str().c_str());
        CallbackData data;
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }

        for (auto result: data.results)
        {
            existingRefIds.insert(std::stoull(result["id"]));
        }
    }

    // ---------------------------------------------------------------------------
    // step 3: get ref_ids for each id
    refIdsOfId.clear();
    {
        std::stringstream ss;
        ss << "SELECT id, ref_ids FROM publications WHERE id IN (";
        for (auto iter = ids.begin(); iter != ids.end(); iter++)
        {
            if (iter != ids.begin())
                ss << ",";
            ss << *iter;
        }
        ss << ");";
        logDebug(ss.str().c_str());
        CallbackData data;
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }

        for (auto result: data.results)
        {
            uint64_t id = std::stoull(result["id"]);
            std::vector<uint64_t> myRefIds;
            std::string refIdsStr = result["ref_ids"];
            std::vector<std::string> refIDStrs = splitString(refIdsStr, ",");
            for (std::string s : refIDStrs)
            {
                uint64_t refId = std::stoull(s);
                if (existingRefIds.find(refId) != existingRefIds.end())
                {
                    myRefIds.push_back(refId);
                }
            }
            refIdsOfId[id] = myRefIds;
        }
    }

    sqlite3_close(db);
    return true;
}

void ResearchScope::writeWoS(int yb, int ye, std::string fileName)
{
    std::ofstream f(fileName);
    for (int y = yb; y < ye; y++)
    {
        std::map<uint64_t, Publication> pubsOfY;
        std::map<uint64_t, Publication> refsOfY;
        if (!load(y, pubsOfY, refsOfY))
        {
            f.close();
            return;
        }
        for (auto idToPub = pubsOfY.begin(); idToPub != pubsOfY.end(); idToPub++)
        {
            idToPub->second.writeWoS(f, refsOfY);
        }
    }
    f.close();
}

bool ResearchScope::remove(const std::string path, const std::string keywords)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError(wxT("Cannot open database at" + path));
        return false;
    }
    CallbackData data;
    char *errorMessage = NULL;

    std::stringstream ss;
    ss << "DELETE FROM research_scopes WHERE keywords = '" << keywords << "';";
    std::string strSql = ss.str();
    CallbackData::updateWriteCount(1, strSql.size());
    logDebug(strSql.c_str());
    rc = sqlite3x_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
    if (rc != SQLITE_OK)
    {
        logError(errorMessage);
    }
    sqlite3_close(db);
    return rc == SQLITE_OK;
}

bool ResearchScope::remove(const std::string keywords)
{
    GeneralConfig config;
    return remove(config.getDatabase(), keywords);
}

bool ResearchScope::remove()
{
    return remove(_path, getKeywords());
}
