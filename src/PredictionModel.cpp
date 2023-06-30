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

bool PredictionModel::load(int y, std::map<uint64_t, std::vector<double>> *prediction)
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
        ss << "SELECT id, scope_keywords, year, rts FROM pub_scope_prediction WHERE scope_keywords = '"
            << keywords << "' AND year = " << y << ";";
        std::string strSql = ss.str();
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
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
            (*prediction)[id] = rts;
        }
    }

    // step 2: load token
    {
        CallbackData data;
        std::stringstream ss;
        ss << "SELECT keywords, year FROM scope_prediction_token WHERE keywords = '"
            << keywords << "' AND year = " << _y2 << ";";
        std::string strSql = ss.str();
        rc = sqlite3_exec(db, strSql.c_str(), CallbackData::sqliteCallback, &data, &errorMessage);
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

bool PredictionModel::save(int y, std::map<uint64_t, std::vector<double>> &prediction)
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
        "PRIMARY KEY(id,scope_keywords,year))",
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

    // step 2: insert publication scope time series
    std::string keywords = _scope.getKeywords();
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO pub_scope_prediction(id, scope_keywords, year, rts) VALUES ";
        for (auto idToTS = prediction.begin(); idToTS != prediction.end(); idToTS++)
        {
            uint64_t id = idToTS->first;
            std::vector<double> rts = idToTS->second;
            std::string strRts = getVectorStr(rts);
            if (idToTS != prediction.begin())
                ss << ",";
            ss << "(" << id << ",'" << keywords << "'," << y << ",'" << strRts << "')";
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

    // insert token
    time_t t;
    time(&t);
    {
        std::stringstream ss;
        ss << "INSERT OR IGNORE INTO scope_prediction_token(keywords,year,update_time) VALUES ('"
            << keywords << "'," << y << "," << (int) t << ");";
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

bool PredictionModel::process(int iStep)
{
    int y = iStep == 0 ? _y2 : _y2 + 5;
    std::map<uint64_t, std::vector<double>> prediction;

    std::map<uint64_t, TSM> timeSeries;
    if (!_tse->load(y, &timeSeries))
        return false;
    for (auto &idToTs: timeSeries)
    {
        std::vector<int> lts = idToTs.second.first.first;
        std::vector<double> lts2(lts.size());
        for (size_t i = 0; i < lts.size(); i++)
        {
            lts2[i] = lts[i];
        }
        TimeSeriesRegression regression(lts2, TimeSeriesRegression::TP_QUADRATIC);
        double cosErr = regression.cosError();
        std::vector<double> rts(5);
        for (int i = 0; i < 5; i++)
        {
            rts[i] = regression.predict(10 + i) * cosErr;
        }
        prediction[idToTs.first] = rts;
    }

    save(y, prediction);


    return true;
}
