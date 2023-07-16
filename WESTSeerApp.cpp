/***************************************************************
 * Name:      WESTSeerApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Junfeng Wu (wujunfeng@vip.163.com)
 * Created:   2023-06-11
 * Copyright: Junfeng Wu (https://westseer.org)
 * License:
 **************************************************************/

#include "WESTSeerApp.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <GeneralConfig.h>
#include <AbstractTask.h>
#include "SettingsDialog.h"
#include <string>
#include <ctime>
#include <OpenAlex.h>
#include <TermExtraction.h>
#include <TermTfIrdf.h>
#include <BitermDf.h>
#include <BitermWeight.h>
#include <CandidateIdentification.h>
#include <TopicIdentification.h>
#include <TimeSeriesExtraction.h>
#include <PredictionModel.h>
#include <MetricModel.h>


//(*AppHeaders
#include "WESTSeerMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(WESTSeerApp);

wxFileConfig *WESTSeerApp::_fileConfig = NULL;
wxFFileLog *WESTSeerApp::_log = NULL;
int WESTSeerApp::_year;
std::thread *WESTSeerApp::_scheduleThread = NULL;
std::atomic_bool WESTSeerApp::_makingSchedule(false);
std::atomic_bool WESTSeerApp::_closing(false);
std::atomic<int> WESTSeerApp::_numScheduledScopes(0);
std::atomic<int> WESTSeerApp::_idxScheduledScope(0);
std::atomic<int> WESTSeerApp::_scheduledScopeProgress(0);

WESTSeerApp::MyProgressReporter::MyProgressReporter()
{
}

void WESTSeerApp::MyProgressReporter::report(const char *taskName, int taskId, int numTasks, int taskProgress)
{
    if (strcmp(taskName, "Done") == 0 || taskId >= numTasks)
        WESTSeerApp::_scheduledScopeProgress = 100;
    else if (strcmp(taskName, "Cancelled") == 0)
        WESTSeerApp::_scheduledScopeProgress = -1;
    else
        WESTSeerApp::_scheduledScopeProgress = (100 * taskId + taskProgress) / numTasks;

    bool paused = false;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);

    if (WESTSeerApp::_scheduledScopeProgress < 100)
        schedule[WESTSeerApp::_idxScheduledScope].second = wxString::Format("%d%%", WESTSeerApp::_scheduledScopeProgress.load());
    else
        schedule[WESTSeerApp::_idxScheduledScope].second = "Done";
    AbstractTask::saveSchedule(schedule, false);
}

WESTSeerApp::MyProgressReporter WESTSeerApp::_progressReporter;

wxFileConfig *WESTSeerApp::getFileConfig()
{
    return _fileConfig;
}

void WESTSeerApp::FlushLog()
{
    _log->Flush();
}

int WESTSeerApp::year()
{
    return _year;
}

bool WESTSeerApp::OnInit()
{
    // get current year
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    _year = now->tm_year + 1900;

    // create app directory if not exists
    wxString appDir = wxStandardPaths::Get().GetUserLocalDataDir();
	if (!wxDirExists(appDir))
    {
        wxMkdir(appDir);
    }

    // create INI file in user's directory
	wxString iniFileName = wxFileName(appDir, "config.ini").GetFullPath();
	_fileConfig = new wxFileConfig(wxT("WESTSeer"), wxT("westseer.org"), iniFileName);
	wxConfigBase::Set(_fileConfig);

	// start logging
	wxString logFileName(wxFileName(appDir, "log.txt").GetFullPath());
	if (wxFileExists(logFileName))
    {
        wxString oldLogFileName1(wxFileName(appDir, "log.old.1.txt").GetFullPath());
        if (wxFileExists(oldLogFileName1))
        {
            wxString oldLogFileName2(wxFileName(appDir, "log.old.2.txt").GetFullPath());
            if (wxFileExists(oldLogFileName2))
            {
                wxString oldLogFileName3(wxFileName(appDir, "log.old.3.txt").GetFullPath());
                wxRenameFile(oldLogFileName2, oldLogFileName3);
            }
            wxRenameFile(oldLogFileName1, oldLogFileName2);
        }
        wxRenameFile(logFileName, oldLogFileName1);
    }
    _log = new wxFFileLog(logFileName);
    if (_log->IsLogFileOK())
    {
        _logChain = new wxLogChain(_log);
    }
    else
    {
        wxString msg;

        msg.Printf(_("Could not initialize the application log (file \"%s\"). \n\nContinue anyway?"),
            _log->GetLogFileName());
        delete _log;

        if ( wxMessageBox(msg, _("Error"), wxYES_NO | wxICON_ERROR, NULL)
             != wxYES )
        {
            return false;
        }
    }

    // first time init
    std::string email;
    {
        GeneralConfig config;
        email = config.getEmail();
    }
    if (email == "")
    {
        SettingsDialog dlg(NULL);
        dlg.ShowModal();
        GeneralConfig config;
        email = config.getEmail();
        if (email == "")
            return false;
    }

    // start schedule thread
    _scheduleThread = new std::thread(
    []
    {
        OpenAlex *openAlex = NULL;
        TermExtraction *termExtraction = NULL;
        TermTfIrdf *termTfirdf = NULL;
        BitermDf *bitermDf = NULL;
        BitermWeight *bitermWeight = NULL;
        CandidateIdentification *candidateIdentification = NULL;
        TopicIdentification *topicIdentification = NULL;
        TimeSeriesExtraction *timeSeriesExtraction = NULL;
        PredictionModel *predictionModel = NULL;
        MetricModel *metricModel = NULL;

        // check schedule every second
        for (;;)
        {
            if (_scheduledScopeProgress == 100 || _scheduledScopeProgress < 0)
            {
                AbstractTask::finalize();
                _scheduledScopeProgress = 0;
                delete metricModel;
                delete predictionModel;
                delete timeSeriesExtraction;
                delete candidateIdentification;
                delete bitermWeight;
                delete bitermDf;
                delete termTfirdf;
                delete termExtraction;
                delete openAlex;
            }
            if (_closing)
            {
                if (AbstractTask::taskThread() != NULL)
                {
                    AbstractTask::cancel();
                    AbstractTask::finalize();
                    delete metricModel;
                    delete predictionModel;
                    delete timeSeriesExtraction;
                    delete candidateIdentification;
                    delete bitermWeight;
                    delete bitermDf;
                    delete termTfirdf;
                    delete termExtraction;
                    delete openAlex;
                }
                return;
            }
            if (!_makingSchedule && AbstractTask::taskThread() == NULL)
            {
                bool paused = true;
                std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
                WESTSeerApp::_numScheduledScopes = schedule.size();
                if (!paused)
                {
                    _idxScheduledScope = (int)schedule.size();
                    for (int i = 0; i < (int)schedule.size(); i++)
                    {
                        if (schedule[i].second != "Done")
                        {
                            _idxScheduledScope = i;
                            break;
                        }
                    }

                    if (_idxScheduledScope < (int)schedule.size())
                    {
                        AbstractTask::setProgressReporter(&WESTSeerApp::_progressReporter);
                        GeneralConfig config;
                        std::string path = config.getDatabase();
                        std::string kws = schedule[_idxScheduledScope].first;
                        openAlex = new OpenAlex(config.getEmail(), path, kws, false);

                        termExtraction = new TermExtraction(path, kws);
                        openAlex->setNext(termExtraction);

                        termTfirdf = new TermTfIrdf(path, kws, termExtraction);
                        termExtraction->setNext(termTfirdf);

                        bitermDf = new BitermDf(path, kws, termTfirdf);
                        termTfirdf->setNext(bitermDf);

                        bitermWeight = new BitermWeight(path, kws, termTfirdf, bitermDf);
                        bitermDf->setNext(bitermWeight);

                        candidateIdentification = new CandidateIdentification(path, kws);
                        bitermWeight->setNext(candidateIdentification);

                        topicIdentification = new TopicIdentification(path, kws, termExtraction, bitermWeight, candidateIdentification);
                        candidateIdentification->setNext(topicIdentification);

                        timeSeriesExtraction = new TimeSeriesExtraction(path, kws, bitermWeight, candidateIdentification, topicIdentification);
                        topicIdentification->setNext(timeSeriesExtraction);

                        predictionModel = new PredictionModel(path, kws, timeSeriesExtraction);
                        timeSeriesExtraction->setNext(predictionModel);

                        metricModel = new MetricModel(path, kws, timeSeriesExtraction, predictionModel);
                        predictionModel->setNext(metricModel);

                        openAlex->runAll();
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    );

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	WESTSeerFrame* Frame = new WESTSeerFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}

int WESTSeerApp::OnExit()
{
    if (_scheduleThread)
    {
        _closing = true;
        _scheduleThread->join();
        delete _scheduleThread;
        _scheduleThread = NULL;
    }

    if(_fileConfig) {
		_fileConfig->Flush();
		delete _fileConfig;
		_fileConfig=NULL;
	}
	if (_logChain) {
        wxLog::SetActiveTarget(NULL);
        delete _logChain;
        _log = NULL;
	}
	wxConfigBase::Set(NULL);
	return 0;
}
