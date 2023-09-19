#include "PredictionModel.h"
#include <../WESTSeerApp.h>
#include <GeneralConfig.h>
#include <StringProcessing.h>
#include <CallbackData.h>
#include <TimeSeriesRegression.h>

PredictionModel::PredictionModel(const std::string path, const std::string kws, TimeSeriesExtraction *tse):_scope(path, kws)
{
    //ctor
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;
    _tse = tse;
}

PredictionModel::~PredictionModel()
{
    //dtor
}

bool PredictionModel::finished()
{
    if (!load(_y2, NULL))
        return false;
    if (!load(_y2 + 5, NULL))
        return false;
    return true;
}

const char *PredictionModel::name()
{
    return "Prediction";
}

int PredictionModel::numSteps()
{
    return 2;
}

void PredictionModel::doStep(int stepId)
{
    process(stepId);
}

bool PredictionModel::load(const std::string keywords, int y, std::map<uint64_t, std::pair<std::vector<double>,std::vector<double>>> *prediction)
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

    if (prediction != NULL)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT id, scope_keywords, year, rts, topic_rts FROM pub_scope_prediction WHERE scope_keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        rc = sqlite3x_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        for (auto result: data.results)
        {
            uint64_t id = std::stoull(result["id"]);
            std::vector<double> rts = getDoubleVector(result["rts"]);
            std::vector<double> topicRts = getDoubleVector(result["topic_rts"]);
            std::pair<std::vector<double>,std::vector<double>> myPrediction(rts,topicRts);
            (*prediction)[id] = myPrediction;
        }
    }

    return true;
}

bool PredictionModel::load(int y, std::map<uint64_t, std::pair<std::vector<double>,std::vector<double>>> *prediction)
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
    if (prediction != NULL)
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT id, scope_keywords, year, rts, topic_rts FROM pub_scope_prediction WHERE scope_keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        rc = sqlite3x_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logDebug(errorMessage);
            sqlite3_close(db);
            return false;
        }
        for (auto result: data.results)
        {
            uint64_t id = std::stoull(result["id"]);
            std::vector<double> rts = getDoubleVector(result["rts"]);
            std::vector<double> topicRts = getDoubleVector(result["topic_rts"]);
            std::pair<std::vector<double>,std::vector<double>> myPrediction(rts,topicRts);
            (*prediction)[id] = myPrediction;
        }
    }

    // step 2: load token
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_prediction_token WHERE keywords = '"
            << keywords << "' AND year = " << _y2 << ";";
        std::string strSql = ss.str();
        rc = sqlite3x_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
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

bool PredictionModel::save(int y, std::map<uint64_t, std::pair<std::vector<double>,std::vector<double>>> &prediction)
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
        "CREATE TABLE IF NOT EXISTS scope_prediction_token("
        "keywords TEXT,"
        "year INTEGER,"
        "update_time INTEGER,"
        "PRIMARY KEY(keywords,year));",

        "CREATE TABLE IF NOT EXISTS pub_scope_prediction("
        "id INTEGER,"
        "scope_keywords TEXT,"
        "year INTEGER,"
        "rts TEXT,"
        "topic_rts TEXT,"
        "PRIMARY KEY(id,scope_keywords,year))",
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

    // step 2: insert publication scope time series
    std::string keywords = _scope.getKeywords();
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO pub_scope_prediction(id, scope_keywords, year, rts, topic_rts) VALUES ";
        for (auto idToTS = prediction.begin(); idToTS != prediction.end(); idToTS++)
        {
            uint64_t id = idToTS->first;
            std::vector<double> rts = idToTS->second.first;
            std::vector<double> topicRts = idToTS->second.second;
            std::string strRts = getVectorStr(rts);
            std::string strTopicRts = getVectorStr(topicRts);
            if (idToTS != prediction.begin())
                ss << ",";
            ss << "(" << id << ",'" << keywords << "'," << y << ",'" << strRts << "','" << strTopicRts << "')";
        }
        ss << ";";
        std::string strSql = ss.str();
        CallbackData::updateWriteCount(prediction.size(), strSql.size());
        logDebug(strSql.c_str());
        rc = sqlite3x_exec(db, strSql.c_str(), NULL, NULL, &errorMessage);
        if (rc != SQLITE_OK)
        {
            logError(errorMessage);
            sqlite3_close(db);
            return false;
        }
    }

    // insert token
    time_t t;
    time(&t);
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_prediction_token(keywords,year,update_time) VALUES ('"
            << keywords << "'," << y << "," << (int) t << ");";
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

bool PredictionModel::process(int iStep)
{
    int y = iStep == 0 ? _y2 : _y2 + 5;
    std::map<uint64_t, std::pair<std::vector<double>,std::vector<double>>> prediction;

    std::map<uint64_t, TSM> timeSeries;
    if (!_tse->load(y, &timeSeries))
        return false;
    if (cancelled())
        return false;
    for (auto &idToTs: timeSeries)
    {
        std::vector<int> lts = idToTs.second.first.first;
        std::vector<int> topicLts = idToTs.second.second.first;
        std::vector<double> lts2(lts.size()), topicLts2(lts.size());
        for (size_t i = 0; i < lts.size(); i++)
        {
            lts2[i] = lts[i];
            topicLts2[i] = topicLts[i];
        }
        TimeSeriesRegression regression(lts2, TimeSeriesRegression::TP_QUADRATIC);
        TimeSeriesRegression topicRegression(topicLts2, TimeSeriesRegression::TP_QUADRATIC);
        double cosErr = regression.cosError();
        double topicCosErr = topicRegression.cosError();
        std::vector<double> rts(5), topicRts(5);
        for (int i = 0; i < 5; i++)
        {
            rts[i] = regression.predict(10 + i) * cosErr;
            topicRts[i] = topicRegression.predict(10 + i) * topicCosErr;
        }
        std::pair<std::vector<double>,std::vector<double>> myPrediction(rts,topicRts);
        prediction[idToTs.first] = myPrediction;
        if (cancelled())
            return false;
    }

    if (cancelled())
        return false;
    save(y, prediction);


    return true;
}

bool PredictionModel::removeOneYear(const std::string keywords, int y)
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
        ss << "DELETE FROM pub_scope_prediction WHERE year = " << y << " AND scope_keywords = '" << keywords << "';";
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
        ss << "DELETE FROM scope_prediction_token WHERE year = " << y << " AND keywords = '" << keywords << "';";
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
