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

#include <wx/app.h>
#include <wx/fileconf.h>
#include <wxFFileLog.h>

class WESTSeerApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        static wxFileConfig *getFileConfig();
        static void FlushLog();
        static int year();
    private:
        static wxFileConfig *_fileConfig;
        static wxFFileLog *_log;
        static int _year;
        wxLogChain *_logChain;
};

#endif // WESTSEERAPP_H
