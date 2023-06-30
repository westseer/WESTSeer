#ifndef WXFFILELOG_H_INCLUDED
#define WXFFILELOG_H_INCLUDED

#include <string>
#include <wx/wx.h>
#include <wx/log.h>
#include <wx/datetime.h>
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

class wxFFileLog : public wxLog
{
public:
    wxFFileLog(const wxString& fileName)
    {
        wxLogNull ln;
        if ( m_logFile.Open(fileName, "a+") )
            m_logFile.Write(wxString::Format("(%s) === LOGGING STARTED ===\n",
                wxDateTime::Now().FormatISOCombined(' ')));
    }

    ~wxFFileLog()
    {
        if ( IsLogFileOK() )
            m_logFile.Write(wxString::Format("(%s) === LOGGING FINISHED ===\n\n",
                wxDateTime::Now().FormatISOCombined(' ')));
    }

    virtual void Flush()
    {
        if ( m_logFile.IsOpened() )
            m_logFile.Flush();
    }

    bool IsLogFileOK() const { return m_logFile.IsOpened() && !m_logFile.Error(); }
    wxString GetLogFileName() const { return m_logFile.GetName(); }

protected:
    virtual void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
    {
        if ( !IsLogFileOK() )
            return;

        wxString prefix;

        prefix.Printf("(%s) ", wxDateTime(info.timestamp).FormatISOCombined(' '));

        switch ( level )
        {
            case wxLOG_Error:
                prefix += "ERROR: ";
                break;

            case wxLOG_Warning:
                prefix += "Warning: ";
                break;

            case wxLOG_Message:
                prefix += "Message: ";
                break;

            case wxLOG_Status:
                prefix += "Status: ";
                break;

            case wxLOG_Info:
                prefix += "Info: ";
                break;

            case wxLOG_Debug:
                prefix += "Debug: ";
                break;

            case wxLOG_Trace:
                prefix += "Trace: ";
                break;

            default:
                prefix += "Other: ";
        }

        m_logFile.Write(wxString::Format("%s%s [%s in %s(%d)]\n", prefix, msg, info.func, info.filename, info.line));
    }

private:
    wxFFile  m_logFile;
};

#define logError wxLogError
#ifdef __DEBUG__
#define logWarning wxLogWarning
#define logVerbose wxLogVerbose
#define logDebug wxLogDebug
#define logMessage wxLogMessage
#else
#define logWarning(x)
#define logVerbose(x)
#define logDebug(x)
#define logMessage(x)
#endif

#endif // WXFFILELOG_H_INCLUDED
