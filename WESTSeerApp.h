/***************************************************************
 * Name:      WESTSeerApp.h
 * Purpose:   Defines Application Class
 * Author:    Junfeng Wu (wujunfeng@vip.163.com)
 * Created:   2023-06-11
 * Copyright: Junfeng Wu (https://westseer.org)
 * License:
 **************************************************************/

#ifndef WESTSEERAPP_H
#define WESTSEERAPP_H

#include <ProgressReporter.h>
#include <wx/app.h>
#include <wx/fileconf.h>
#include <wxFFileLog.h>
#include <thread>
#include <atomic>

class WESTSeerApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        static wxFileConfig *getFileConfig();
        static void FlushLog();
        static int year();
        static std::atomic_bool _makingSchedule;

    private:
        static wxFileConfig *_fileConfig;
        static wxFFileLog *_log;
        static int _year;
        static std::thread *_scheduleThread;
        static std::atomic_bool _closing;
        static std::atomic<int> _numScheduledScopes;
        static std::atomic<int> _idxScheduledScope;
        static std::atomic<int> _scheduledScopeProgress;

        static class MyProgressReporter: public ProgressReporter
        {
            public:
                MyProgressReporter();
                virtual void report(const char *taskName, int taskId, int numTasks, int taskProgress);
        } _progressReporter;

        wxLogChain *_logChain;
};

#endif // WESTSEERAPP_H
