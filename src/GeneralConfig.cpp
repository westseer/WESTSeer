#include "GeneralConfig.h"
#include "../WESTSeerApp.h"

GeneralConfig::GeneralConfig()
{
    //ctor
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/General");
    wxString email = config->Read("Email","");
    _email = email.ToStdString();
    wxString appDir = wxStandardPaths::Get().GetUserLocalDataDir();
    wxString dbFileName(wxFileName(appDir, "database.sqlite").GetFullPath());
    wxString database = config->Read("Database",dbFileName);
    _database = database.ToStdString();
    _obYears = 30;
    config->Read("ObYears", &_obYears);
    _biterms = 5;
    config->Read("Biterms", &_biterms);
    _tfirdf = 0.3;
    config->Read("TFIRDF", &_tfirdf);
    _citations = 20;
    config->Read("Citations", &_citations);
}

GeneralConfig::~GeneralConfig()
{
    //dtor
}

void GeneralConfig::setEmail(std::string value)
{
    _email = value;
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/General");
    wxString email(_email);
    config->Write("Email", email);
}

void GeneralConfig::setDatabase(std::string value)
{
    _database = value;
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/General");
    wxString database(_database);
    config->Write("Database", database);
}

void GeneralConfig::setBiterms(int value)
{
    _biterms = value;
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/General");
    config->Write("Biterms", _biterms);
}

void GeneralConfig::setTfirdf(double value)
{
    _tfirdf = value;
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/General");
    config->Write("TFIRDF", _tfirdf);
}

void GeneralConfig::setCitations(int value)
{
    _citations = value;
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/General");
    config->Write("Citations", _citations);
}

const std::string GeneralConfig::getLogFile()
{
    wxString appDir = wxStandardPaths::Get().GetUserLocalDataDir();
    wxString logFileName(wxFileName(appDir, "log.txt").GetFullPath());
    return logFileName.ToStdString();
}
