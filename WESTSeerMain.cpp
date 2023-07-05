/***************************************************************
 * Name:      WESTSeerMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Junfeng Wu (wujunfeng@vip.163.com)
 * Created:   2023-06-11
 * Copyright: Junfeng Wu (https://westseer.org)
 * License:
 **************************************************************/

#include "WESTSeerMain.h"
#include "WESTSeerApp.h"
#include "OpenAlexImportDialog.h"
#include "SettingsDialog.h"
#include "SQLDialog.h"
#include "LogDialog.h"
#include "FinishedScopeDialog.h"
#include <GeneralConfig.h>
#include <StringProcessing.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <fstream>

//(*InternalHeaders(WESTSeerFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(WESTSeerFrame)
const long WESTSeerFrame::ID_STATICTEXT1 = wxNewId();
const long WESTSeerFrame::ID_CHOICE1 = wxNewId();
const long WESTSeerFrame::ID_BUTTON1 = wxNewId();
const long WESTSeerFrame::ID_STATICTEXT2 = wxNewId();
const long WESTSeerFrame::ID_GAUGE1 = wxNewId();
const long WESTSeerFrame::ID_BUTTON2 = wxNewId();
const long WESTSeerFrame::ID_STATICTEXT7 = wxNewId();
const long WESTSeerFrame::ID_GAUGE2 = wxNewId();
const long WESTSeerFrame::ID_BUTTON3 = wxNewId();
const long WESTSeerFrame::ID_LISTCTRL1 = wxNewId();
const long WESTSeerFrame::ID_LISTCTRL2 = wxNewId();
const long WESTSeerFrame::ID_LISTCTRL3 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL1 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL2 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL3 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL4 = wxNewId();
const long WESTSeerFrame::ID_NOTEBOOK1 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM3 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM4 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM5 = wxNewId();
const long WESTSeerFrame::idMenuQuit = wxNewId();
const long WESTSeerFrame::ID_MENUITEM2 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM1 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM8 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM6 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM7 = wxNewId();
const long WESTSeerFrame::idMenuAbout = wxNewId();
const long WESTSeerFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(WESTSeerFrame,wxFrame)
    //(*EventTable(WESTSeerFrame)
    //*)
END_EVENT_TABLE()

WESTSeerFrame::WESTSeerFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(WESTSeerFrame)
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;

    Create(parent, wxID_ANY, _("Worldwide Emerging Scientific Topic Seer "), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(800,600));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("westseer-logo.ico"))));
    	SetIcon(FrameIcon);
    }
    FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(1);
    FlexGridSizer1->AddGrowableRow(2);
    FlexGridSizer2 = new wxFlexGridSizer(3, 3, 0, 0);
    FlexGridSizer2->AddGrowableCol(1);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Scope:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer2->Add(StaticText1, 1, wxALL|wxEXPAND, 5);
    ChoiceScope = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    FlexGridSizer2->Add(ChoiceScope, 1, wxALL|wxEXPAND, 5);
    ButtonNew = new wxButton(this, ID_BUTTON1, _("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer2->Add(ButtonNew, 1, wxALL|wxEXPAND, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Overall Progress:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer2->Add(StaticText2, 1, wxALL|wxEXPAND, 5);
    GaugeOverall = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAUGE1"));
    FlexGridSizer2->Add(GaugeOverall, 1, wxALL|wxEXPAND, 5);
    ButtonPause = new wxButton(this, ID_BUTTON2, _("Pause"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    ButtonPause->Disable();
    FlexGridSizer2->Add(ButtonPause, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT7, _("Step Progress:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer2->Add(StaticText3, 1, wxALL|wxEXPAND, 5);
    GaugeStep = new wxGauge(this, ID_GAUGE2, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAUGE2"));
    FlexGridSizer2->Add(GaugeStep, 1, wxALL|wxEXPAND, 5);
    ButtonResume = new wxButton(this, ID_BUTTON3, _("Resume"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    ButtonResume->Disable();
    FlexGridSizer2->Add(ButtonResume, 1, wxALL|wxEXPAND, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
    ListCtrlPublications = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
    FlexGridSizer1->Add(ListCtrlPublications, 1, wxALL|wxEXPAND, 5);
    NotebookInfo = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    ListCtrlReferences = new wxListCtrl(NotebookInfo, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL2"));
    ListCtrlCitations = new wxListCtrl(NotebookInfo, ID_LISTCTRL3, wxPoint(225,140), wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL3"));
    TextCtrlAbstract = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    TextCtrlTopic = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    TextCtrlPrediction = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL3, _("Text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    TextCtrlVerification = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL4, _("Text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    NotebookInfo->AddPage(ListCtrlReferences, _("References"), false);
    NotebookInfo->AddPage(ListCtrlCitations, _("Citations"), false);
    NotebookInfo->AddPage(TextCtrlAbstract, _("Abstract"), false);
    NotebookInfo->AddPage(TextCtrlTopic, _("Topic"), false);
    NotebookInfo->AddPage(TextCtrlPrediction, _("Prediction Summary"), false);
    NotebookInfo->AddPage(TextCtrlVerification, _("Verification Summary"), false);
    FlexGridSizer1->Add(NotebookInfo, 1, wxALL|wxEXPAND, 5);
    SetSizer(FlexGridSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItemOptions = new wxMenuItem(Menu1, ID_MENUITEM3, _("&Options"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItemOptions);
    MenuItem6 = new wxMenuItem(Menu1, ID_MENUITEM4, _("&Export Web-of-Science Data"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem6);
    MenuItem7 = new wxMenuItem(Menu1, ID_MENUITEM5, _("&Save Results"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu3, ID_MENUITEM2, _("&Test"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem4);
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("&Explore"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem3);
    MenuBar1->Append(Menu3, _("&Mode"));
    Menu4 = new wxMenu();
    MenuItem5 = new wxMenuItem(Menu4, ID_MENUITEM8, _("&Finished Scopes"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem5);
    MenuItemSQL = new wxMenuItem(Menu4, ID_MENUITEM6, _("&SQL"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItemSQL);
    MenuItemLog = new wxMenuItem(Menu4, ID_MENUITEM7, _("&Log"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItemLog);
    MenuBar1->Append(Menu4, _("&View"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("&About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    SetSizer(FlexGridSizer1);
    Layout();

    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnChoiceScopeSelect);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&WESTSeerFrame::OnButtonNewClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&WESTSeerFrame::OnButtonPauseClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&WESTSeerFrame::OnButtonResumeClick);
    Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnListCtrlPublicationsItemSelect);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuItemOptionsSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnExportWoSSelected);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnSaveResultsSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnQuit);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnTextModeSelected);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnExploreModeSelected);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuFinishedScopesSelected);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuItemSQLSelected);
    Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuItemLogSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnAbout);
    //*)

    _openAlex = NULL;
    _termExtraction = NULL;
    _termTfirdf = NULL;
    _bitermDf = NULL;
    _bitermWeight = NULL;
    _candidateIdentification = NULL;
    _topicIdentification = NULL;
    _timeSeriesExtraction = NULL;
    _predictionModel = NULL;
    _metricModel = NULL;
    _exploreMode = false;
    _displayThread = NULL;
    _progressReporter = new MyProgressReporter(this);
    GeneralConfig config;
    std::vector<std::string> scopes = ResearchScope::getResearchScopes(config.getDatabase());
    for (std::string scope: scopes)
    {
        ChoiceScope->Append(scope);
    }
}

WESTSeerFrame::~WESTSeerFrame()
{
    //(*Destroy(WESTSeerFrame)
    //*)
}

void WESTSeerFrame::showCandidate(uint64_t id)
{
    if (_metricModel == NULL || _topicIdentification == NULL || _timeSeriesExtraction == NULL || _timeSeries.size() == 0)
        return;

    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _displayThread = new std::thread([this, id]{
        ListCtrlPublications->Disable();
        NotebookInfo->Disable();
        StatusBar1->SetStatusText("Publication Info Retrieval (0%)", 1);

        GeneralConfig config;
        std::string path = config.getDatabase();
        std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
        ResearchScope scope(path, kws);
        Publication pub = scope.getPublication(id);
        int ye = _exploreMode ? WESTSeerApp::year() + 5 : WESTSeerApp::year();
        Publication me = scope.getPublication(id);
        std::pair<std::string,std::string> topic = _topics[id];
        {
            std::stringstream ss;
            ss << "Topic: " << simplifyTopic(topic.second) << std::endl;
            auto idToTSM = _timeSeries.find(id);
            if (idToTSM != _timeSeries.end())
            {
                std::vector<int> topicLts = idToTSM->second.second.first;
                std::vector<int> topicRts = idToTSM->second.second.second;
                for (int i = 0; i < 10; i++)
                {
                    ss << ye - 15 + i << ":" << topicLts[i] << std::endl;
                }
                for (int i = 0; i < 5; i++)
                {
                    ss << ye - 5 + i << ":" << topicRts[i] << std::endl;
                }
            }
            TextCtrlTopic->SetValue(ss.str());
        }
        StatusBar1->SetStatusText("Publication Info Retrieval (20%)", 1);

        TextCtrlAbstract->SetValue(me.abstract());
        {
            auto idToScore = _scores.find(id);
            auto idToTSM = _timeSeries.find(id);
            if (idToScore != _scores.end() && idToTSM != _timeSeries.end())
            {
                std::vector<int> rts = idToTSM->second.first.second;
                std::stringstream ss;
                ss << "predicted score = " << idToScore->second[0] << std::endl
                    << "citations on topic: " << std::endl;
                for (int i = 0; i < 15; i++)
                {
                    if (i < 10)
                    {
                        ss << ye - 15 + i << ": " << idToScore->second[2 + i] << std::endl;
                    }
                    else
                    {
                        ss << ye - 15 + i << ": " << rts[i - 10] << "(" << idToScore->second[2 + i] << " predicted)" << std::endl;
                    }
                }
                TextCtrlPrediction->SetValue(ss.str().c_str());
            }
            if (idToScore != _scores.end() && !_exploreMode)
            {
                std::stringstream ss;
                ss << "verified score = " << idToScore->second[1] << std::endl
                    << "citations: " << std::endl;
                for (int i = 0; i < 15; i++)
                {
                    ss << ye - 15 + i << ": " << idToScore->second[17 + i] << std::endl;
                }
                TextCtrlVerification->SetValue(ss.str().c_str());
            }
        }
        StatusBar1->SetStatusText("Publication Info Retrieval (60%)", 1);

        ListCtrlCitations->ClearAll();
        ListCtrlCitations->AppendColumn("Year");
        ListCtrlCitations->AppendColumn("Title", wxLIST_FORMAT_LEFT, 600);
        ListCtrlCitations->AppendColumn("Authors");
        ListCtrlCitations->AppendColumn("Source");
        ListCtrlCitations->AppendColumn("ID");
        std::vector<Publication> citations = _citations[id];
        for (size_t i = 0; i < citations.size(); i++)
        {
            Publication pub = citations[i];
            ListCtrlCitations->InsertItem(i, wxString::Format("%d", pub.year()));
            ListCtrlCitations->SetItem(i, 1, pub.title().c_str());
            std::stringstream ssAuthors;
            std::vector<wxString> authors = pub.authors();
            for (size_t j = 0; j < authors.size(); j++)
            {
                if (j > 0)
                    ssAuthors << ",";
                ssAuthors << authors[j].ToStdString();
            }
            ListCtrlCitations->SetItem(i, 2, ssAuthors.str().c_str());
            ListCtrlCitations->SetItem(i, 3, pub.source().c_str());
            ListCtrlCitations->SetItem(i, 4, wxString::Format("%llu", pub.id()));
        }
        StatusBar1->SetStatusText("Publication Info Retrieval (80%)", 1);

        ListCtrlReferences->ClearAll();
        ListCtrlReferences->AppendColumn("Year");
        ListCtrlReferences->AppendColumn("Title", wxLIST_FORMAT_LEFT, 600);
        ListCtrlReferences->AppendColumn("Authors");
        ListCtrlReferences->AppendColumn("Source");
        ListCtrlReferences->AppendColumn("ID");
        std::vector<Publication> references = scope.getReferences(id);
        for (size_t i = 0; i < references.size(); i++)
        {
            Publication pub = references[i];
            ListCtrlReferences->InsertItem(i, wxString::Format("%d", pub.year()));
            ListCtrlReferences->SetItem(i, 1, pub.title().c_str());
            std::stringstream ssAuthors;
            std::vector<wxString> authors = pub.authors();
            for (size_t j = 0; j < authors.size(); j++)
            {
                if (j > 0)
                    ssAuthors << ",";
                ssAuthors << authors[j].ToStdString();
            }
            ListCtrlReferences->SetItem(i, 2, ssAuthors.str().c_str());
            ListCtrlReferences->SetItem(i, 3, pub.source().c_str());
            ListCtrlReferences->SetItem(i, 4, wxString::Format("%llu", pub.id()));
        }
        ListCtrlPublications->Enable();
        NotebookInfo->Enable();
        StatusBar1->SetStatusText("Publication Info Retrieval (100%)", 1);
    });
}

void WESTSeerFrame::showCandidates()
{
    if (_candidateIdentification == NULL || _metricModel == NULL || _timeSeriesExtraction == NULL)
        return;

    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _displayThread = new std::thread([this]{
        ListCtrlPublications->Disable();
        NotebookInfo->Disable();

        _ids.clear();
        _vRanks.clear();
        _timeSeries.clear();
        _topics.clear();
        _scores.clear();
        _citations.clear();

        StatusBar1->SetStatusText("Result Info Retrieval (0%)", 1);

        ListCtrlPublications->AppendColumn("Year");
        ListCtrlPublications->AppendColumn("Title", wxLIST_FORMAT_LEFT, 600);
        ListCtrlPublications->AppendColumn("Authors");
        ListCtrlPublications->AppendColumn("Source");
        ListCtrlPublications->AppendColumn("ID");
        ListCtrlPublications->AppendColumn("P-Score");
        ListCtrlPublications->AppendColumn("P-Rank");
        if (!_exploreMode)
        {
            ListCtrlPublications->AppendColumn("V-Score");
            ListCtrlPublications->AppendColumn("V-Rank");
        }
        ListCtrlPublications->AppendColumn("RPYS1-NC");
        ListCtrlPublications->AppendColumn("RPYS1-NTop10");
        ListCtrlPublications->AppendColumn("RPYS1-NTop5");
        ListCtrlPublications->AppendColumn("RPYS1-NTop1");
        ListCtrlPublications->AppendColumn("RPYS2-NC");
        ListCtrlPublications->AppendColumn("RPYS2-NTop10");
        ListCtrlPublications->AppendColumn("RPYS2-NTop5");
        ListCtrlPublications->AppendColumn("RPYS2-NTop1");
        ListCtrlPublications->AppendColumn("NC");
        int ye = _exploreMode ? WESTSeerApp::year() + 5 : WESTSeerApp::year();
        if (!_timeSeriesExtraction->load(ye, &_timeSeries))
        {
            logError("cannot load time series");
            return;
        }
        StatusBar1->SetStatusText("Result Info Retrieval (10%)", 1);

        if (!_topicIdentification->load(ye, &_topics))
        {
            logError("cannot load topics");
            return;
        }
        StatusBar1->SetStatusText("Result Info Retrieval (20%)", 1);

        if (!_metricModel->load(ye, &_scores))
        {
            logError("cannot load scores");
            return;
        }
        StatusBar1->SetStatusText("Result Info Retrieval (30%)", 1);

        GeneralConfig config;
        std::string path = config.getDatabase();
        std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
        ResearchScope scope(path, kws);
        std::vector<uint64_t> ids;
        std::vector<int> pRanks, vRanks;
        if (_candidateIdentification->load(ye, &ids))
        {
            StatusBar1->SetStatusText("Result Info Retrieval (35%)", 1);

            pRanks.resize(ids.size());
            vRanks.resize(ids.size());
            for (size_t i = 0; i < ids.size(); i++)
            {
                int myPRank = 0, myVRank = 0;
                for (size_t j = 0; j < ids.size(); j++)
                {
                    if (j == i)
                        continue;

                    if (_scores[ids[j]][0] > _scores[ids[i]][0])
                        myPRank++;
                    else if (_scores[ids[j]][0] == _scores[ids[i]][0] && j < i)
                        myPRank++;

                    if (_scores[ids[j]][1] > _scores[ids[i]][1])
                        myVRank++;
                    else if (_scores[ids[j]][1] == _scores[ids[i]][1] && j < i)
                        myVRank++;
                }
                pRanks[i] = myPRank;
                vRanks[i] = myVRank;
            }
            _ids.resize(ids.size());
            _vRanks.resize(ids.size());
            for (size_t i = 0; i < ids.size(); i++)
            {
                _ids[pRanks[i]] = ids[i];
                _vRanks[pRanks[i]] = vRanks[i];
            }
            _citations = scope.getCitations(_ids, ye);
            StatusBar1->SetStatusText("Result Info Retrieval (40%)", 1);

            std::vector<Publication> pubs = scope.getPublications(_ids);
            std::map<uint64_t,Publication> pubMap;
            for (Publication pub: pubs)
            {
                int progress0 = 40 + 50 * pubMap.size() / pubs.size();
                pubMap[pub.id()] = pub;
                int progress1 = 40 + 50 * pubMap.size() / pubs.size();
                if (progress1 > progress0)
                {
                    wxString s = wxString::Format("Result Info Retrieval (%d)", progress1);
                    StatusBar1->SetStatusText(s, 1);
                }
            }
            StatusBar1->SetStatusText("Result Info Retrieval (90%)", 1);

            for (int i = 0; i < (int)_ids.size() ; i++)
            {
                Publication pub = pubMap[_ids[i]];
                ListCtrlPublications->InsertItem(i, wxString::Format("%d", pub.year()));
                ListCtrlPublications->SetItem(i, 1, pub.title().c_str());
                std::stringstream ssAuthors;
                std::vector<wxString> authors = pub.authors();
                for (size_t i = 0; i < authors.size(); i++)
                {
                    if (i > 0)
                        ssAuthors << ",";
                    ssAuthors << authors[i].ToStdString();
                }
                ListCtrlPublications->SetItem(i, 2, ssAuthors.str().c_str());
                ListCtrlPublications->SetItem(i, 3, pub.source().c_str());
                ListCtrlPublications->SetItem(i, 4, wxString::Format("%llu", pub.id()));
                double pScore = _scores[pub.id()][0];
                double vScore = _scores[pub.id()][1];
                int rpys1NC = (int)_scores[pub.id()][32];
                int rpys1NTop10 = (int)_scores[pub.id()][33];
                int rpys1NTop5 = (int)_scores[pub.id()][34];
                int rpys1NTop1 = (int)_scores[pub.id()][35];
                int rpys2NC = (int)_scores[pub.id()][36];
                int rpys2NTop10 = (int)_scores[pub.id()][37];
                int rpys2NTop5 = (int)_scores[pub.id()][38];
                int rpys2NTop1 = (int)_scores[pub.id()][39];
                int nc = 0;
                for (int j = 0; j < 10; j++)
                {
                    nc += (int)_scores[pub.id()][17 + j];
                }
                ListCtrlPublications->SetItem(i, 5, wxString::Format("%lf", pScore));
                ListCtrlPublications->SetItem(i, 6, wxString::Format("%d", (int)i));
                if (_exploreMode)
                {
                    ListCtrlPublications->SetItem(i, 7, wxString::Format("%d", rpys1NC));
                    ListCtrlPublications->SetItem(i, 8, wxString::Format("%d", rpys1NTop10));
                    ListCtrlPublications->SetItem(i, 9, wxString::Format("%d", rpys1NTop5));
                    ListCtrlPublications->SetItem(i, 10, wxString::Format("%d", rpys1NTop1));
                    ListCtrlPublications->SetItem(i, 11, wxString::Format("%d", rpys2NC));
                    ListCtrlPublications->SetItem(i, 12, wxString::Format("%d", rpys2NTop10));
                    ListCtrlPublications->SetItem(i, 13, wxString::Format("%d", rpys2NTop5));
                    ListCtrlPublications->SetItem(i, 14, wxString::Format("%d", rpys2NTop1));
                    ListCtrlPublications->SetItem(i, 15, wxString::Format("%d", nc));
                }
                else
                {
                    ListCtrlPublications->SetItem(i, 7, wxString::Format("%lf", vScore));
                    ListCtrlPublications->SetItem(i, 8, wxString::Format("%d", _vRanks[i]));
                    ListCtrlPublications->SetItem(i, 9, wxString::Format("%d", rpys1NC));
                    ListCtrlPublications->SetItem(i, 10, wxString::Format("%d", rpys1NTop10));
                    ListCtrlPublications->SetItem(i, 11, wxString::Format("%d", rpys1NTop5));
                    ListCtrlPublications->SetItem(i, 12, wxString::Format("%d", rpys1NTop1));
                    ListCtrlPublications->SetItem(i, 13, wxString::Format("%d", rpys2NC));
                    ListCtrlPublications->SetItem(i, 14, wxString::Format("%d", rpys2NTop10));
                    ListCtrlPublications->SetItem(i, 15, wxString::Format("%d", rpys2NTop5));
                    ListCtrlPublications->SetItem(i, 16, wxString::Format("%d", rpys2NTop1));
                    ListCtrlPublications->SetItem(i, 17, wxString::Format("%d", nc));
                }
                int progress0 = 90 + 10 * i / (int)_ids.size();
                int progress1 = 90 + 10 * (i + 1) / (int)_ids.size();
                if (progress1 > progress0)
                {
                    wxString s = wxString::Format("Result Info Retrieval (%d\%)", progress1);
                    StatusBar1->SetStatusText(s, 1);
                }
            }
        }

        ListCtrlPublications->Enable();
        NotebookInfo->Enable();
    });
}

void WESTSeerFrame::saveCandidates()
{
    int nCols = ListCtrlPublications->GetColumnCount();
    int nRows = ListCtrlPublications->GetItemCount();
    if (nCols == 0 || nRows == 0)
        return;

    wxString userDataDir = wxStandardPaths::Get().GetUserDataDir();
    wxFileDialog openFileDialog(this, _("Save results to tsv file."), userDataDir, "results.csv", "csv files (*.csv)|*.csv", wxFD_SAVE);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    std::string csvFileName = openFileDialog.GetPath().ToStdString();
    ofstream f(csvFileName);
    for (int iCol = 0; iCol < nCols; iCol++)
    {
        if (iCol > 0)
            f << ",";
        wxListItem item;
        item.SetMask(wxLIST_MASK_TEXT);
        ListCtrlPublications->GetColumn(iCol, item);
        wxString colName = item.GetText();
        f << colName.c_str();
    }
    f << std::endl;

    for (int iRow = 0; iRow < nRows; iRow++)
    {
        for (int iCol = 0; iCol < nCols; iCol++)
        {
            if (iCol > 0)
                f << ",";
            wxListItem item;
            wxString text = ListCtrlPublications->GetItemText(iRow, iCol);
            text.Replace(",","_");
            f << text.c_str();
        }
        f << std::endl;
    }
    f.close();
}

void WESTSeerFrame::clearCandidates()
{
    ListCtrlPublications->ClearAll();
    ListCtrlCitations->ClearAll();
    ListCtrlReferences->ClearAll();
    TextCtrlAbstract->SetValue("");
    TextCtrlPrediction->SetValue("");
    TextCtrlVerification->SetValue("");
    TextCtrlTopic->SetValue("");
    _ids.clear();
}

WESTSeerFrame::MyProgressReporter::MyProgressReporter(WESTSeerFrame *frame)
{
    _frame = frame;
    _frame->StatusBar1->SetFieldsCount(3);
    _frame->GaugeOverall->SetRange(100);
    _frame->GaugeStep->SetRange(100);
}

void WESTSeerFrame::MyProgressReporter::report(
    const char *taskName, int taskId, int numTasks, int taskProgress)
{
    std::stringstream ss1;
    ss1 << "task " << taskId << " of " << numTasks;
    _frame->StatusBar1->SetStatusText(ss1.str().c_str(), 0);
    _frame->StatusBar1->SetStatusText(taskName, 1);
    std::stringstream ss2;
    ss2 << taskProgress << "%(" << _frame->getElapseSeconds() << " seconds)";
    _frame->StatusBar1->SetStatusText(ss2.str().c_str(), 2);
    _frame->GaugeStep->SetValue(taskProgress);
    _frame->GaugeOverall->SetValue(100 * taskId / numTasks);

    if (strcmp(taskName, "Done") == 0 || strcmp(taskName, "Cancelled") == 0)
    {
        if (strcmp(taskName, "Done") == 0)
        {
            _frame->showCandidates();
        }
        _frame->ButtonPause->Disable();
        _frame->ButtonResume->Disable();
        _frame->ChoiceScope->Enable();
        _frame->ButtonNew->Enable();
    }
    else
    {
        _frame->ButtonPause->Enable();
        _frame->ButtonResume->Disable();
        _frame->ChoiceScope->Disable();
        _frame->ButtonNew->Disable();
    }
}

void WESTSeerFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void WESTSeerFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void WESTSeerFrame::OnButtonNewClick(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }
    // pop up an OpenAlexImportDialog
    OpenAlexImportDialog dlg(this, -1);
    if (dlg.ShowModal())
    {
        if (dlg._openAlex != NULL)
        {
            // find whether it is in the old choices
            std::string keywords = dlg._openAlex->scope().getKeywords();
            int idxKW = -1;
            for (unsigned int i = 0; i < ChoiceScope->GetCount(); i++)
            {
                if (ChoiceScope->GetString(i).ToStdString() == keywords)
                {
                    idxKW = (int) i;
                    break;
                }
            }
            if (idxKW >= 0)
            {
                ChoiceScope->SetSelection(idxKW);
            }
            else
            {
                idxKW = ChoiceScope->Append(keywords);
                ChoiceScope->SetSelection(idxKW);
            }
            OnChoiceScopeSelect(event);
        }
    }
}

void WESTSeerFrame::OnMenuItemOptionsSelected(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }
    SettingsDialog dlg(this);
    dlg.ShowModal();
}

void WESTSeerFrame::OnMenuItemSQLSelected(wxCommandEvent& event)
{
    SQLDialog dlg(this);
    dlg.ShowModal();
}

void WESTSeerFrame::OnMenuItemLogSelected(wxCommandEvent& event)
{
    LogDialog dlg(this);
    dlg.ShowModal();
}

void WESTSeerFrame::OnChoiceScopeSelect(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    clearCandidates();
    clearScope();
    AbstractTask::setProgressReporter(_progressReporter);
    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    time(&_timeStart);
    _openAlex = new OpenAlex(config.getEmail(), path, kws);

    _termExtraction = new TermExtraction(path, kws);
    _openAlex->setNext(_termExtraction);

    _termTfirdf = new TermTfIrdf(path, kws, _termExtraction);
    _termExtraction->setNext(_termTfirdf);

    _bitermDf = new BitermDf(path, kws, _termTfirdf);
    _termTfirdf->setNext(_bitermDf);

    _bitermWeight = new BitermWeight(path, kws, _termTfirdf, _bitermDf);
    _bitermDf->setNext(_bitermWeight);

    _candidateIdentification = new CandidateIdentification(path, kws);
    _bitermWeight->setNext(_candidateIdentification);

    _topicIdentification = new TopicIdentification(path, kws, _termExtraction, _bitermWeight, _candidateIdentification);
    _candidateIdentification->setNext(_topicIdentification);

    _timeSeriesExtraction = new TimeSeriesExtraction(path, kws, _bitermWeight, _candidateIdentification, _topicIdentification);
    _topicIdentification->setNext(_timeSeriesExtraction);

    _predictionModel = new PredictionModel(path, kws, _timeSeriesExtraction);
    _timeSeriesExtraction->setNext(_predictionModel);

    _metricModel = new MetricModel(path, kws, _timeSeriesExtraction, _predictionModel);
    _predictionModel->setNext(_metricModel);

    _openAlex->runAll();
}

void WESTSeerFrame::clearScope()
{
    _ids.clear();
    _vRanks.clear();
    _timeSeries.clear();
    _topics.clear();
    _scores.clear();
    if (_openAlex != NULL)
    {
        delete _openAlex;
        _openAlex = NULL;
    }
    if (_termExtraction != NULL)
    {
        delete _termExtraction;
        _termExtraction = NULL;
    }
    if (_termTfirdf != NULL)
    {
        delete _termTfirdf;
        _termTfirdf = NULL;
    }
    if (_bitermDf != NULL)
    {
        delete _bitermDf;
        _bitermDf = NULL;
    }
    if (_bitermWeight != NULL)
    {
        delete _bitermWeight;
        _bitermWeight = NULL;
    }
    if (_candidateIdentification != NULL)
    {
        delete _candidateIdentification;
        _candidateIdentification = NULL;
    }
    if (_topicIdentification != NULL)
    {
        delete _topicIdentification;
        _topicIdentification = NULL;
    }
    if (_timeSeriesExtraction != NULL)
    {
        delete _timeSeriesExtraction;
        _timeSeriesExtraction = NULL;
    }
    if (_predictionModel != NULL)
    {
        delete _predictionModel;
        _predictionModel = NULL;
    }
    if (_metricModel != NULL)
    {
        delete _metricModel;
        _metricModel = NULL;
    }
}

void WESTSeerFrame::OnButtonPauseClick(wxCommandEvent& event)
{
    AbstractTask::setProgressReporter(NULL);
    AbstractTask::cancel();
    AbstractTask::finalize();
    ButtonResume->Enable();
}

void WESTSeerFrame::OnButtonResumeClick(wxCommandEvent& event)
{
    OnChoiceScopeSelect(event);
}

void WESTSeerFrame::OnExploreModeSelected(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }
    _exploreMode = true;
    clearCandidates();
    showCandidates();
}

void WESTSeerFrame::OnTextModeSelected(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }
    _exploreMode = false;
    clearCandidates();
    showCandidates();
}

void WESTSeerFrame::OnListCtrlPublicationsItemSelect(wxListEvent& event)
{
    int idxItem = (int)ListCtrlPublications->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxItem >= 0 && idxItem < (int) _ids.size())
        showCandidate(_ids[idxItem]);
}

void WESTSeerFrame::OnExportWoSSelected(wxCommandEvent& event)
{
    wxString userDataDir = wxStandardPaths::Get().GetUserDataDir();
    wxFileDialog openFileDialog(this, _("Save to WoS file"), userDataDir, "wos.txt", "WoS files (*.txt)|*.txt", wxFD_SAVE);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;
    std::string wosFileName = openFileDialog.GetPath().ToStdString();
    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    ResearchScope scope(path, kws);
    if (_exploreMode)
    {
        int ye = WESTSeerApp::year();
        int yb = ye - 15;
        scope.writeWoS(yb, ye, wosFileName);
    }
    else
    {
        int ye = WESTSeerApp::year() - 5;
        int yb = ye - 10;
        scope.writeWoS(yb, ye, wosFileName);
    }
}

void WESTSeerFrame::OnSaveResultsSelected(wxCommandEvent& event)
{
    saveCandidates();
}

void WESTSeerFrame::OnNotebookInfoPageChanged(wxNotebookEvent& event)
{
}

int WESTSeerFrame::getElapseSeconds()
{
    time_t t;
    time(&t);
    return t - _timeStart;
}

void WESTSeerFrame::OnMenuFinishedScopesSelected(wxCommandEvent& event)
{
    FinishedScopeDialog dlg(this);
    dlg.ShowModal();
}
