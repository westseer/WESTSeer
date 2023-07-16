#include "AbstractTask.h"
#include <sstream>
#include <StringProcessing.h>
#include "../WESTSeerApp.h"

std::atomic<bool> AbstractTask::_cancelled(false);
std::thread *AbstractTask::_taskThread(NULL);
ProgressReporter *AbstractTask::_progressReporter(NULL);

AbstractTask::AbstractTask()
{
    //ctor
    _prev = NULL;
    _next = NULL;
}

AbstractTask::~AbstractTask()
{
    //dtor
    finalize();
}

void AbstractTask::finalize()
{
    if (_taskThread != NULL)
    {
        _taskThread->join();
        delete _taskThread;
        _taskThread = NULL;
    }
    _cancelled.store(false);
}

void AbstractTask::runAll()
{
    finalize();
    if (numSteps() > 0 && !finished())
    {
        _taskThread = new std::thread([this]()
            {
                AbstractTask *task = this;
                int taskId = 0;
                int nTasks = getNumTasks();
                while (task != NULL)
                {
                    if (_cancelled.load() == true)
                    {
                        if (_progressReporter != NULL)
                        {
                            _progressReporter->report("Cancelled", taskId, nTasks, 0);
                        }
                        _cancelled.store(false);
                        return;
                    }
                    if (_progressReporter != NULL)
                    {
                        _progressReporter->report(task->name(), taskId, nTasks, 0);
                    }
                    int n = task->numSteps();
                    for (int stepId = 0; stepId < n; stepId++)
                    {
                        task->doStep(stepId);
                        int taskProgress = 100 * (stepId + 1) / n;

                        if (_progressReporter != NULL)
                        {
                            if (_cancelled.load() == true)
                                _progressReporter->report("Cancelled", taskId, nTasks, taskProgress);
                            else
                                _progressReporter->report(task->name(), taskId, nTasks, taskProgress);
                        }
                        if (_cancelled.load() == true)
                        {
                            _cancelled.store(false);
                            return;
                        }
                    }
                    task = task->_next;
                    taskId++;
                }
                if (_progressReporter != NULL)
                    _progressReporter->report("Done", nTasks, nTasks, 0);
            });
    }
    else if (_next != NULL)
    {
        _next->runAll();
    }
    else if (_progressReporter != NULL)
    {
        _progressReporter->report("Done", getNumTasks(), getNumTasks(), 100);
    }
}

void AbstractTask::cancel()
{
    if (_taskThread != NULL)
    {
        _cancelled.store(true);
    }
}

void AbstractTask::setProgressReporter(ProgressReporter *value)
{
    _progressReporter = value;
}

std::vector<std::pair<std::string,std::string>> AbstractTask::loadSchedule(bool *paused)
{
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/Schedule");
    wxString strStatus = config->Read("Status", "");
    std::vector<std::string> temp1 = splitString(strStatus.ToStdString(), "!");
    std::vector<std::pair<std::string,std::string>> schedule;
    for (std::string temp2: temp1)
    {
        std::vector<std::string> temp3 = splitString(temp2, ":");
        std::pair<std::string,std::string> task(temp3[0],temp3[1]);
        schedule.push_back(task);
    }
    config->Read("Paused", paused);
    return schedule;
}

bool AbstractTask::saveSchedule(std::vector<std::pair<std::string,std::string>> &schedule, bool paused)
{
    wxFileConfig *config = WESTSeerApp::getFileConfig();
    config->SetPath("/Schedule");
    std::string strStatus;
    {
        std::stringstream ss;
        for (size_t i = 0; i < schedule.size(); i++)
        {
            if (i > 0)
                ss << "!";
            ss << schedule[i].first << ":" << schedule[i].second;
        }
        strStatus = ss.str();
    }
    config->Write("Status", strStatus.c_str());
    config->Write("Paused", paused);
    return true;
}
