/***************************************************************
 * Name:      WESTSeerMain.h
 * Purpose:   Defines Application Frame
 * Author:    Junfeng Wu (wujunfeng@vip.163.com)
 * Created:   2023-06-11
 * Copyright: Junfeng Wu (https://westseer.org)
 * License:
 **************************************************************/

#ifndef WESTSEERMAIN_H
#define WESTSEERMAIN_H

//(*Headers(WESTSeerFrame)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
//*)

#include <ProgressReporter.h>
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

class WESTSeerFrame: public wxFrame
{
    public:

        WESTSeerFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~WESTSeerFrame();

    private:

        //(*Handlers(WESTSeerFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButtonNewClick(wxCommandEvent& event);
        void OnMenuItemOptionsSelected(wxCommandEvent& event);
        void OnMenuItemSQLSelected(wxCommandEvent& event);
        void OnMenuItemLogSelected(wxCommandEvent& event);
        void OnChoiceScopeSelect(wxCommandEvent& event);
        void OnButtonPauseClick(wxCommandEvent& event);
        void OnButtonResumeClick(wxCommandEvent& event);
        void OnExploreModeSelected(wxCommandEvent& event);
        void OnTextModeSelected(wxCommandEvent& event);
        void OnListCtrlPublicationsItemSelect(wxListEvent& event);
        void OnExportWoSSelected(wxCommandEvent& event);
        void OnSaveResultsSelected(wxCommandEvent& event);
        void OnNotebookInfoPageChanged(wxNotebookEvent& event);
        //*)

        //(*Identifiers(WESTSeerFrame)
        static const long ID_STATICTEXT1;
        static const long ID_CHOICE1;
        static const long ID_BUTTON1;
        static const long ID_STATICTEXT2;
        static const long ID_GAUGE1;
        static const long ID_BUTTON2;
        static const long ID_STATICTEXT7;
        static const long ID_GAUGE2;
        static const long ID_BUTTON3;
        static const long ID_LISTCTRL1;
        static const long ID_LISTCTRL2;
        static const long ID_LISTCTRL3;
        static const long ID_TEXTCTRL1;
        static const long ID_TEXTCTRL2;
        static const long ID_TEXTCTRL3;
        static const long ID_TEXTCTRL4;
        static const long ID_NOTEBOOK1;
        static const long ID_MENUITEM3;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM5;
        static const long idMenuQuit;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM6;
        static const long ID_MENUITEM7;
        static const long ID_MENUITEM8;
        static const long ID_MENUITEM9;
        static const long ID_MENUITEM10;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(WESTSeerFrame)
        wxButton* ButtonNew;
        wxButton* ButtonPause;
        wxButton* ButtonResume;
        wxChoice* ChoiceScope;
        wxGauge* GaugeOverall;
        wxGauge* GaugeStep;
        wxListCtrl* ListCtrlCitations;
        wxListCtrl* ListCtrlPublications;
        wxListCtrl* ListCtrlReferences;
        wxMenu* Menu3;
        wxMenu* Menu4;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem5;
        wxMenuItem* MenuItem6;
        wxMenuItem* MenuItem7;
        wxMenuItem* MenuItemBackward;
        wxMenuItem* MenuItemForward;
        wxMenuItem* MenuItemLog;
        wxMenuItem* MenuItemOptions;
        wxMenuItem* MenuItemSQL;
        wxNotebook* NotebookInfo;
        wxStaticText* StaticText1;
        wxStaticText* StaticText2;
        wxStaticText* StaticText3;
        wxStatusBar* StatusBar1;
        wxTextCtrl* TextCtrlAbstract;
        wxTextCtrl* TextCtrlPrediction;
        wxTextCtrl* TextCtrlTopic;
        wxTextCtrl* TextCtrlVerification;
        //*)

        DECLARE_EVENT_TABLE()

        class MyProgressReporter: public ProgressReporter
        {
            private:
                WESTSeerFrame *_frame;
            public:
                MyProgressReporter(WESTSeerFrame *frame);
                virtual void report(const char *taskName, int taskId, int numTasks, int taskProgress);
        } *_progressReporter;
        OpenAlex *_openAlex;
        TermExtraction *_termExtraction;
        TermTfIrdf *_termTfirdf;
        BitermDf *_bitermDf;
        BitermWeight *_bitermWeight;
        CandidateIdentification *_candidateIdentification;
        TopicIdentification *_topicIdentification;
        TimeSeriesExtraction *_timeSeriesExtraction;
        PredictionModel *_predictionModel;
        MetricModel *_metricModel;
        bool _exploreMode;
        std::vector<uint64_t> _ids;
        std::vector<int> _vRanks;
        std::map<uint64_t,TSM> _timeSeries;
        std::map<uint64_t, std::pair<std::string,std::string>> _topics;
        std::map<uint64_t, std::vector<double>> _scores;

        void clearScope();
        void showCandidates();
        void showCandidate(uint64_t id);
        void clearCandidates();
        void saveCandidates();
};

#endif // WESTSEERMAIN_H
