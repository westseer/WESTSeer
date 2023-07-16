#include "TermExtraction.h"
#include <../WESTSeerApp.h>
#include <StringProcessing.h>
#include <GeneralConfig.h>
#include <CallbackData.h>
#include <wxFFileLog.h>
#include <porter2_stemmer.h>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <set>
#include <thread>
#include <mutex>
#include <queue>

TermExtraction::TermExtraction(const std::string path, const std::string kws) : _scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
}

TermExtraction::~TermExtraction()
{
    //dtor
}

bool TermExtraction::finished()
{
    for (int y = _y2 - 1; y >= _y0; y--)
    {
        if (!load(y, NULL, false))
            return false;
    }
    return true;
}

const char *TermExtraction::name()
{
    return "Term Extraction";
}

int TermExtraction::numSteps()
{
    return _y2 - _y0;
}

void TermExtraction::doStep(int stepId)
{
    process(_y0 + stepId);
}

// Load title, abstract, and reference titles of the works in the scope and published in year y
bool TermExtraction::load(int y, std::map<uint64_t, std::vector<std::string>> &texts)
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

    // step 1: get ids and reference ids
    std::set<uint64_t> ids;
    std::set<uint64_t> refIds;
    int nCombs = _scope.numCombinations();
    for (int idxComb = 0; idxComb < nCombs; idxComb++)
    {
        data.results.clear();
        std::stringstream ss;
        ss << "SELECT combination, year, ids, ref_ids FROM openalex_queries"
           " WHERE combination = '" << _scope.getCombination(idxComb) << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3x_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
        }
        if (rc != SQLITE_OK || data.results.size() == 0)
        {
            sqlite3_close(db);
            return false;
        }
        std::vector<std::string> idStrs = splitString(data.results[0]["ids"], ",");
        std::vector<std::string> refIdStrs = splitString(data.results[0]["ref_ids"], ",");
        for (std::string idStr: idStrs)
        {
            ids.insert(std::stoull(idStr));
        }
        for (std::string refIdStr: refIdStrs)
        {
            refIds.insert(std::stoull(refIdStr));
        }
    }

    // step 2: load titles, abstracts and refIds of ids
    data.results.clear();
    std::map<uint64_t, std::string> titles;
    std::map<uint64_t, std::string> abstracts;
    std::map<uint64_t, std::set<uint64_t>> workRefIds;
    {
        std::stringstream ss;
        ss << "SELECT id, title, abstract, ref_ids FROM publications WHERE id in (";
        for (auto iter = ids.begin(); iter != ids.end(); iter++)
        {
            if (iter != ids.begin())
                ss << ",";
            ss << *iter;
        }
        ss << ");";
        std::string strSql = ss.str();
        logDebug(strSql.c_str());
        rc = sqlite3x_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        for (auto &result: data.results)
        {
            uint64_t id = std::stoull(result["id"]);
            titles[id] = result["title"];
            abstracts[id] = result["abstract"];
            std::set<uint64_t> myRefIds;
            std::vector<std::string> myRefIdStrs = splitString(result["ref_ids"], ",");
            for (std::string refIdStr: myRefIdStrs)
            {
                myRefIds.insert(std::stoull(refIdStr));
            }
            workRefIds[id] = myRefIds;
        }
    }

    // step 3: load reference titles
    data.results.clear();
    std::map<uint64_t, std::string> refTitles;
    {
        std::stringstream ss;
        ss << "SELECT id, title FROM publications WHERE language = 'en' AND id in (";
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
            uint64_t refId = std::stoull(result["id"]);
            refTitles[refId] = result["title"];
        }
    }

    // step 4: merge texts
    texts.clear();
    for (uint64_t id: ids)
    {
        auto idToTitle = titles.find(id);
        if (idToTitle == titles.end())
            continue;

        std::vector<std::string> text;
        text.push_back(normalize(idToTitle->second));

        auto idToAbstract = abstracts.find(id);
        if (idToAbstract == abstracts.end())
            text.push_back("");
        else
            text.push_back(normalize(idToAbstract->second));

        auto idToRefIds = workRefIds.find(id);
        if (idToRefIds == workRefIds.end())
        {
            texts[id] = text;
            continue;
        }

        for (uint64_t refId: idToRefIds->second)
        {
            auto refIdToRefTitle = refTitles.find(refId);
            if (refIdToRefTitle != refTitles.end())
            {
                text.push_back(normalize(refIdToRefTitle->second));
            }
        }
        texts[id] = text;
    }
    sqlite3_close(db);
    return true;
}

bool TermExtraction::save(int y, const std::map<uint64_t, std::map<std::string, std::pair<std::string, int>>> &termFreqs)
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
        "CREATE TABLE IF NOT EXISTS scope_terms("
        "keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "terms TEXT,"
        "PRIMARY KEY(keywords,year));",

        "CREATE TABLE IF NOT EXISTS pub_scope_terms("
        "id INTEGER,"
        "scope_keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "terms TEXT,"
        "PRIMARY KEY(id,scope_keywords))",
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

    // step 2: save publication scope terms
    std::string keywords = _scope.getKeywords();
    time_t t;
    time(&t);
    if (termFreqs.size() > 0)
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO pub_scope_terms(id, scope_keywords, year, update_time, terms) VALUES ";
        for (auto iter = termFreqs.begin(); iter != termFreqs.end(); iter++)
        {
            if (iter != termFreqs.begin())
                ss << ",";
            ss << "(" << iter->first << ",'" << keywords << "'," << y << "," << (int)t << ",'";
            for (auto termToFreq = iter->second.begin(); termToFreq != iter->second.end(); termToFreq++)
            {
                if (termToFreq != iter->second.begin())
                    ss << ",";
                ss << termToFreq->first << ":" << termToFreq->second.first << ":" << termToFreq->second.second;
            }
            ss << "')";
        }
        ss << ";";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(termFreqs.size(), strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3x_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    // step 3: save scope terms
    {
        std::stringstream ss;
        std::string terms = _matcher.getTerms();
        ss << "INSERT OR IGNORE INTO scope_terms(keywords, year, update_time, terms) VALUES ('"
            << keywords << "'," << y << "," << (int)t <<",'" << terms << "');";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(1, strSql.size());
        logDebug(strSql.c_str());
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

bool TermExtraction::load(const std::string keywords, int y, std::map<uint64_t, std::map<std::string, std::pair<std::string,int>>> *termFreqs)
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

    // step 1: load publication scope terms
    if (termFreqs != NULL)
    {
        termFreqs->clear();
        {
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, year, terms FROM pub_scope_terms WHERE scope_keywords = '"
                << keywords << "' AND year = " << y << ";";
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
                std::map<std::string, std::pair<std::string,int>> myTermFreqs;
                uint64_t id = std::stoull(result["id"]);
                std::string strTermFreqs = result["terms"];
                std::vector<std::string> fields = splitString(strTermFreqs, ",");
                for (std::string field: fields)
                {
                    std::vector<std::string> kv = splitString(field, ":");
                    std::pair<std::string,int> tf(kv[1],atoi(kv[2].c_str()));
                    myTermFreqs[kv[0]] = tf;
                }
                (*termFreqs)[id] = myTermFreqs;
            }
        }
    }
    sqlite3_close(db);
    return true;
}

bool TermExtraction::load(int y, std::map<uint64_t, std::map<std::string, std::pair<std::string, int>>> *termFreqs, bool loadTerms)
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

    // step 1: load publication scope terms
    std::string keywords = _scope.getKeywords();
    if (termFreqs != NULL)
    {
        termFreqs->clear();
        {
            std::stringstream ss;
            ss << "SELECT id, scope_keywords, year, terms FROM pub_scope_terms WHERE scope_keywords = '"
                << keywords << "' AND year = " << y << ";";
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
                std::map<std::string, std::pair<std::string,int>> myTermFreqs;
                uint64_t id = std::stoull(result["id"]);
                std::string strTermFreqs = result["terms"];
                std::vector<std::string> fields = splitString(strTermFreqs, ",");
                for (std::string field: fields)
                {
                    std::vector<std::string> kv = splitString(field, ":");
                    std::pair<std::string,int> tf(kv[1],atoi(kv[2].c_str()));
                    myTermFreqs[kv[0]] = tf;
                }
                (*termFreqs)[id] = myTermFreqs;
            }
        }
    }


    // step 2: load scope terms
    data.results.clear();
    if (loadTerms)
    {
        std::stringstream ss;
        ss << "SELECT keywords, year, terms FROM scope_terms WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        if (data.results.size() > 0)
        {
            std::string terms = data.results[0]["terms"];
            _matcher.load(terms);
        }
    }
    else
    {
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_terms WHERE keywords = '"
            << keywords << "' AND year = " << y << ";";
        logDebug(ss.str().c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
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

// seperate text with puntuations and stop words, unless "-" forces connecting tokens into one term
std::vector<std::vector<std::string>> TermExtraction::split(const std::string text)
{
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> tokens = tokenize(text);
    std::vector<std::string> term;
    size_t idxToken = 0;
    while (idxToken < tokens.size())
    {
        if (tokens[idxToken] == "-")
        {
            // forced connection
            idxToken++;
        }
        else
        {
            std::vector<AbstractMatcher::Type> m = _stopWordMatcher.match(tokens, idxToken);
            if (m.size() > 0)
            {
                // it is a puntuation or stop word
                if (term.size() >0)
                {
                    result.push_back(term);
                    term.clear();
                }
                idxToken += m.size();
            }
            else
            {
                // it is a token in a term
                term.push_back(tokens[idxToken]);
                idxToken++;
            }
        }
    }
    if (term.size() > 0)
    {
        result.push_back(term);
    }
    return result;
}

bool TermExtraction::process(int y)
{
    if (load(y + 1, NULL, false))
        return true;
    if (load(y, NULL, true))
        return true;

    // step 1:load texts
    std::map<uint64_t, std::vector<std::string>> texts;
    if (!load(y, texts))
        return false;
    if (_cancelled.load() == true)
    {
        return false;
    }

    // step 2: extract terms
    for (auto iter = texts.begin(); iter != texts.end(); iter++)
    {
        std::vector<std::string> &texts = iter->second;
        for (size_t idxText = 0; idxText < 2 && idxText < texts.size(); idxText++)
        {
            std::vector<std::vector<std::string>> terms = split(texts[idxText]);
            for (size_t idxTerm = 0; idxTerm < terms.size(); idxTerm++)
            {
                std::vector<std::string> &term = terms[idxTerm];
                _matcher.insertTerm(term, 0);
            }
        }
        if (_cancelled.load() == true)
        {
            return false;
        }
    }

    // step 3: flexible extraction of terms in titles, abstracts and reference titles
    std::queue<uint64_t> q;
    for (auto iter = texts.begin(); iter != texts.end(); iter++)
    {
        q.push(iter->first);
    }
    int nThreads = std::thread::hardware_concurrency();
    std::thread *threads[nThreads];
    std::mutex mq;
    std::map<uint64_t, std::map<std::string, std::pair<std::string, int>>> termFreqs;

    for (int tid = 0; tid < nThreads; tid++)
    {
        threads[tid] = new std::thread([&q, &mq, &texts, this, &termFreqs]
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

                    auto iter = texts.find(id);
                    std::vector<std::string> &texts = iter->second;
                    std::map<std::string, std::map<std::string,int>> termFreqsOfWork;
                    for (size_t idxText = 0; idxText < texts.size(); idxText++)
                    {
                        std::vector<std::vector<std::string>> terms = split(texts[idxText]);
                        for (size_t idxTerm = 0; idxTerm < terms.size(); idxTerm++)
                        {
                            std::vector<std::string> &term = terms[idxTerm];
                            for (size_t i = 0; i < term.size(); i++)
                            {
                                std::vector<AbstractMatcher::Type> m = _matcher.match(term, i);
                                std::string s;
                                std::string t;
                                for (size_t j = 0; j < m.size(); j++)
                                {
                                    if (j > 0)
                                    {
                                        s += " ";
                                        t += " ";
                                    }
                                    std::string token = term[i + j];
                                    t += token;
                                    Porter2Stemmer::stem(token);
                                    s += token;
                                    if (m[j] == AbstractMatcher::TERM)
                                    {
                                        auto sToTFreq = termFreqsOfWork.find(s);
                                        if (sToTFreq != termFreqsOfWork.end())
                                        {
                                            auto tToFreq = sToTFreq->second.find(t);
                                            if (tToFreq != sToTFreq->second.end())
                                            {
                                                termFreqsOfWork[s][t] = tToFreq->second + 1;
                                            }
                                            else
                                            {
                                                termFreqsOfWork[s][t] = 1;
                                            }
                                        }
                                        else
                                        {
                                            std::map<std::string,int> temp;
                                            temp[t] = 1;
                                            termFreqsOfWork[s] = temp;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    std::map<std::string, std::pair<std::string, int>> stf;
                    for (auto &sToTf: termFreqsOfWork)
                    {
                        int sumF = 0;
                        int maxF = 0;
                        std::string tMaxF = "";
                        for (auto &tToF: sToTf.second)
                        {
                            sumF += tToF.second;
                            if (tToF.second > maxF)
                            {
                                maxF = tToF.second;
                                tMaxF = tToF.first;
                            }
                        }
                        std::pair<std::string,int> tf(tMaxF,sumF);
                        stf[sToTf.first] = tf;
                    }
                    {
                        std::lock_guard<std::mutex> lock(mq);
                        termFreqs[id] = stf;
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

    if (_cancelled.load() == true || termFreqs.size() < texts.size())
        return false;
    // step 4: save extraction results
    save(y, termFreqs);
    if (y == _y2 - 1)
    {
        _matcher.clear();
        _stopWordMatcher.clear();
    }
    return true;
}

bool TermExtraction::removeOneYear(const std::string keywords, int y)
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
        ss << "DELETE FROM pub_scope_terms WHERE year = " << y << " AND scope_keywords = '" << keywords << "';";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(1, strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
        }
    }

    {
        std::stringstream ss;
        ss << "DELETE FROM scope_terms WHERE year = " << y << " AND keywords = '" << keywords << "';";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(1, strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3x_exec(db, ss.str().c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
        }
    }

    sqlite3_close(db);
    return rc == SQLITE_OK;
}
