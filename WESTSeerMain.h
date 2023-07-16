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
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)

#include <map>
#include <string>
#include <TimeSeriesExtraction.h>

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
        void OnExploreModeSelected(wxCommandEvent& event);
        void OnTextModeSelected(wxCommandEvent& event);
        void OnListCtrlPublicationsItemSelect(wxListEvent& event);
        void OnExportWoSSelected(wxCommandEvent& event);
        void OnSaveResultsSelected(wxCommandEvent& event);
        void OnNotebookInfoPageChanged(wxNotebookEvent& event);
        void OnMenuViewDownloadsSelected(wxCommandEvent& event);
        void OnMenuViewSchedulesSelected(wxCommandEvent& event);
        void OnMenuAddScopeSelected(wxCommandEvent& event);
        void OnTimer1Trigger(wxTimerEvent& event);
        void OnChoiceHL1Select(wxCommandEvent& event);
        void OnChoiceHL2Select(wxCommandEvent& event);
        void OnListCtrlPublicationsItemRClick(wxListEvent& event);
        void OnListCtrlCitationsItemRClick(wxListEvent& event);
        void OnButtonRefreshClick(wxCommandEvent& event);
        void OnRadioBox1Select(wxCommandEvent& event);
        void OnRadioBox2Select(wxCommandEvent& event);
        //*)

        //(*Identifiers(WESTSeerFrame)
        static const long ID_STATICTEXT1;
        static const long ID_CHOICE1;
        static const long ID_BUTTON1;
        static const long ID_STATICTEXT2;
        static const long ID_CHOICE2;
        static const long ID_RADIOBOX1;
        static const long ID_STATICTEXT3;
        static const long ID_CHOICE3;
        static const long ID_RADIOBOX2;
        static const long ID_LISTCTRL1;
        static const long ID_LISTCTRL2;
        static const long ID_LISTCTRL3;
        static const long ID_TEXTCTRL1;
        static const long ID_TEXTCTRL2;
        static const long ID_TEXTCTRL3;
        static const long ID_TEXTCTRL4;
        static const long ID_NOTEBOOK1;
        static const long ID_MENUITEM10;
        static const long ID_MENUITEM3;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM5;
        static const long idMenuQuit;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM9;
        static const long ID_MENUITEM8;
        static const long ID_MENUITEM6;
        static const long ID_MENUITEM7;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(WESTSeerFrame)
        wxButton* ButtonRefresh;
        wxChoice* ChoiceHL1;
        wxChoice* ChoiceHL2;
        wxChoice* ChoiceScope;
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
        wxMenuItem* MenuItem8;
        wxMenuItem* MenuItem9;
        wxMenuItem* MenuItemLog;
        wxMenuItem* MenuItemOptions;
        wxMenuItem* MenuItemSQL;
        wxNotebook* NotebookInfo;
        wxRadioBox* RadioBox1;
        wxRadioBox* RadioBox2;
        wxStaticText* StaticText1;
        wxStaticText* StaticText2;
        wxStaticText* StaticText3;
        wxStatusBar* StatusBar1;
        wxTextCtrl* TextCtrlAbstract;
        wxTextCtrl* TextCtrlPrediction;
        wxTextCtrl* TextCtrlTopic;
        wxTextCtrl* TextCtrlVerification;
        wxTimer Timer1;
        //*)

        DECLARE_EVENT_TABLE()

        bool _exploreMode;
        std::vector<uint64_t> _ids;
        std::vector<int> _vRanks;
        std::map<uint64_t,TSM> _timeSeries;
        std::map<uint64_t, std::pair<std::string,std::string>> _topics;
        std::map<uint64_t, std::vector<double>> _scores;
        std::map<uint64_t, std::vector<Publication>> _citations;
        int _numTermGroups;
        int _numBitermGroups;
        std::map<uint64_t, std::map<uint64_t, std::vector<double>>> _termHLs;
        std::map<uint64_t, std::map<uint64_t, std::vector<double>>> _bitermHLs;
        std::map<uint64_t, std::map<uint64_t, double>> _hlws1, _hlws2;
        std::thread *_displayThread;

        void showCandidates();
        void highlightCandidates();
        void highlightCitations(uint64_t id, int idxColor1, int idxColor2);
        void showCandidate(uint64_t id);
        void clearCandidates();
        void saveCandidates();
};

#endif // WESTSEERMAIN_H
