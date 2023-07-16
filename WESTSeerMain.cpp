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
#include "DownloadsDialog.h"
#include "SchedulesDialog.h"
#include <GeneralConfig.h>
#include <StringProcessing.h>
#include <ResearchScope.h>
#include <CallbackData.h>
#include <OpenAlex.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <fstream>
#include <MetricModel.h>

//(*InternalHeaders(WESTSeerFrame)
#include <wx/bitmap.h>
#include <wx/font.h>
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
const long WESTSeerFrame::ID_CHOICE2 = wxNewId();
const long WESTSeerFrame::ID_RADIOBOX1 = wxNewId();
const long WESTSeerFrame::ID_STATICTEXT3 = wxNewId();
const long WESTSeerFrame::ID_CHOICE3 = wxNewId();
const long WESTSeerFrame::ID_RADIOBOX2 = wxNewId();
const long WESTSeerFrame::ID_LISTCTRL1 = wxNewId();
const long WESTSeerFrame::ID_LISTCTRL2 = wxNewId();
const long WESTSeerFrame::ID_LISTCTRL3 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL1 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL2 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL3 = wxNewId();
const long WESTSeerFrame::ID_TEXTCTRL4 = wxNewId();
const long WESTSeerFrame::ID_NOTEBOOK1 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM10 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM3 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM4 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM5 = wxNewId();
const long WESTSeerFrame::idMenuQuit = wxNewId();
const long WESTSeerFrame::ID_MENUITEM2 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM1 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM9 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM8 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM6 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM7 = wxNewId();
const long WESTSeerFrame::idMenuAbout = wxNewId();
const long WESTSeerFrame::ID_STATUSBAR1 = wxNewId();
const long WESTSeerFrame::ID_TIMER1 = wxNewId();
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
    ChoiceScope = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_CHOICE1"));
    FlexGridSizer2->Add(ChoiceScope, 1, wxALL|wxEXPAND, 5);
    ButtonRefresh = new wxButton(this, ID_BUTTON1, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer2->Add(ButtonRefresh, 1, wxALL|wxEXPAND, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Highlight 1:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer2->Add(StaticText2, 1, wxALL|wxEXPAND, 5);
    ChoiceHL1 = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_CHOICE2"));
    FlexGridSizer2->Add(ChoiceHL1, 1, wxALL|wxEXPAND, 5);
    wxString __wxRadioBoxChoices_1[3] =
    {
    	_("R"),
    	_("G"),
    	_("B")
    };
    RadioBox1 = new wxRadioBox(this, ID_RADIOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 1, wxRA_VERTICAL, wxDefaultValidator, _T("ID_RADIOBOX1"));
    RadioBox1->SetSelection(1);
    wxFont RadioBox1Font(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("@System"),wxFONTENCODING_DEFAULT);
    RadioBox1->SetFont(RadioBox1Font);
    FlexGridSizer2->Add(RadioBox1, 1, wxALL|wxEXPAND, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Highlight 2:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer2->Add(StaticText3, 1, wxALL|wxEXPAND, 5);
    ChoiceHL2 = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_CHOICE3"));
    FlexGridSizer2->Add(ChoiceHL2, 1, wxALL|wxEXPAND, 5);
    wxString __wxRadioBoxChoices_2[3] =
    {
    	_("R"),
    	_("G"),
    	_("B")
    };
    RadioBox2 = new wxRadioBox(this, ID_RADIOBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_2, 1, wxRA_VERTICAL, wxDefaultValidator, _T("ID_RADIOBOX2"));
    RadioBox2->SetSelection(2);
    wxFont RadioBox2Font(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("@System"),wxFONTENCODING_DEFAULT);
    RadioBox2->SetFont(RadioBox2Font);
    FlexGridSizer2->Add(RadioBox2, 1, wxALL|wxEXPAND, 5);
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
    MenuItem9 = new wxMenuItem(Menu1, ID_MENUITEM10, _("&Add Scope"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem9);
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
    MenuItem8 = new wxMenuItem(Menu4, ID_MENUITEM9, _("&Schedules"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem8);
    MenuItem5 = new wxMenuItem(Menu4, ID_MENUITEM8, _("&Downloads"), wxEmptyString, wxITEM_NORMAL);
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
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(1000, false);
    SetSizer(FlexGridSizer1);
    Layout();

    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnChoiceScopeSelect);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&WESTSeerFrame::OnButtonRefreshClick);
    Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnChoiceHL1Select);
    Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnRadioBox1Select);
    Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnChoiceHL2Select);
    Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnRadioBox2Select);
    Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnListCtrlPublicationsItemSelect);
    Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&WESTSeerFrame::OnListCtrlPublicationsItemRClick);
    Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&WESTSeerFrame::OnListCtrlCitationsItemRClick);
    Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuAddScopeSelected);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuItemOptionsSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnExportWoSSelected);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnSaveResultsSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnQuit);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnTextModeSelected);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnExploreModeSelected);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuViewSchedulesSelected);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuViewDownloadsSelected);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuItemSQLSelected);
    Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnMenuItemLogSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&WESTSeerFrame::OnTimer1Trigger);
    //*)

    _exploreMode = false;
    _numTermGroups = 0;
    _numBitermGroups = 0;
    _displayThread = NULL;
    GeneralConfig config;
    std::vector<std::string> scopes = MetricModel::getScopesWithMetrics(config.getDatabase());
    for (std::string scope: scopes)
    {
        ChoiceScope->Append(scope);
    }
    StatusBar1->SetFieldsCount(9);
}

WESTSeerFrame::~WESTSeerFrame()
{
    //(*Destroy(WESTSeerFrame)
    //*)
}

void WESTSeerFrame::showCandidate(uint64_t id)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _displayThread = new std::thread([this, id]{
        int idxColor1 = RadioBox1->GetSelection();
        int idxColor2 = RadioBox2->GetSelection();

        ListCtrlPublications->Disable();
        NotebookInfo->Disable();
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
        highlightCitations(id, idxColor1, idxColor2);
        ListCtrlPublications->Enable();
        NotebookInfo->Enable();
    });
}

void WESTSeerFrame::showCandidates()
{
    if (ChoiceScope->GetSelection() <0)
        return;

    std::string keywords = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    if (keywords.size() == 0)
        return;

    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _displayThread = new std::thread([this, keywords]{
        ListCtrlPublications->Disable();
        NotebookInfo->Disable();

        _ids.clear();
        _vRanks.clear();
        _timeSeries.clear();
        _topics.clear();
        _scores.clear();
        _citations.clear();
        _termHLs.clear();
        _bitermHLs.clear();
        _hlws1.clear();
        _hlws2.clear();
        ListCtrlPublications->ClearAll();
        ListCtrlReferences->ClearAll();
        ListCtrlCitations->ClearAll();

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
        if (!TimeSeriesExtraction::load(keywords, ye, &_timeSeries))
        {
            logError("cannot load time series");
            return;
        }

        if (!TopicIdentification::load(keywords, ye, &_topics))
        {
            logError("cannot load topics");
            return;
        }

        if (!MetricModel::load(keywords, ye, &_scores, &_termHLs, &_bitermHLs))
        {
            logError("cannot load scores");
            return;
        }
        std::vector<std::string> kwGroups = splitString(keywords,";");
        _numTermGroups = kwGroups.size();
        _numBitermGroups = _numTermGroups * (_numTermGroups - 1) / 2;
        ChoiceHL1->Clear();
        ChoiceHL2->Clear();
        for (size_t i = 0; i < kwGroups.size(); i++)
        {
            ChoiceHL1->Append(kwGroups[i].c_str());
            ChoiceHL2->Append(kwGroups[i].c_str());
        }
        for (size_t i1 = 0; i1 < kwGroups.size(); i1++)
        {
            for (size_t i2 = i1 + 1; i2 < kwGroups.size(); i2++)
            {
                std::string temp = kwGroups[i1] + ";" + kwGroups[i2];
                ChoiceHL1->Append(temp.c_str());
                ChoiceHL2->Append(temp.c_str());
            }
        }

        GeneralConfig config;
        std::string path = config.getDatabase();
        std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
        ResearchScope scope(path, kws);
        std::vector<uint64_t> ids;
        std::vector<int> pRanks, vRanks;
        if (CandidateIdentification::load(keywords, ye, &ids))
        {
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
            std::map<uint64_t,std::vector<uint64_t>> cids;
            for (auto idToCs: _citations)
            {
                std::vector<uint64_t> myCids;
                for (Publication &pub: idToCs.second)
                {
                    myCids.push_back(pub.id());
                }
                cids[idToCs.first] = myCids;
            }

            std::vector<Publication> pubs = scope.getPublications(_ids);
            std::map<uint64_t,Publication> pubMap;
            for (Publication pub: pubs)
            {
                pubMap[pub.id()] = pub;
            }

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
            }
        }

        ListCtrlPublications->Enable();
        NotebookInfo->Enable();
    });
}

wxColor getColor(int idxColor1, int idxColor2, double h1, double h2)
{
    double h = h1 > h2 ? h1 : h2;
    int r = 255, g = 255, b = 255;
    if (idxColor1 == idxColor2)
    {
        switch (idxColor1)
        {
        case 0:
            {
                r = (int)round(h * 255);
                g = (int)round((1 - h) * 255);
                b = (int)round((1 - h) * 255);
            }
            break;
        case 1:
            {
                r = (int)round((1 - h) * 255);
                g = (int)round(h * 255);
                b = (int)round((1 - h) * 255);
            }
            break;
        default:
            {
                r = (int)round((1 - h) * 255);
                g = (int)round((1 - h) * 255);
                b = (int)round(h * 255);
            }
        }
    }
    else
    {
        if (0 != idxColor1 && 0 != idxColor2)
        {
            r = (int)round((1 - h) * 255);
        }
        else if (1 != idxColor1 && 1 != idxColor2)
        {
            g = (int)round((1 - h) * 255);
        }
        else if (2 != idxColor1 && 2 != idxColor2)
        {
            b = (int)round((1 - h) * 255);
        }
        switch (idxColor1)
        {
        case 0:
            r = (int)round(h1 * 255);
            break;
        case 1:
            g = (int)round(h1 * 255);
            break;
        default:
            b = (int)round(h1 * 255);
            break;
        }
        switch (idxColor2)
        {
        case 0:
            r = (int)round(h2 * 255);
            break;
        case 1:
            g = (int)round(h2 * 255);
            break;
        default:
            b = (int)round(h2 * 255);
            break;
        }
    }
    return wxColor(r,g,b);
}

void WESTSeerFrame::highlightCitations(uint64_t id, int idxColor1, int idxColor2)
{
    auto idToH1 = _hlws1.find(id);
    auto idToH2 = _hlws2.find(id);
    int numCitations = ListCtrlCitations->GetItemCount();
    for (int idxCitation = 0; idxCitation < numCitations; idxCitation++)
    {
        uint64_t cid = std::stoull(ListCtrlCitations->GetItemText(idxCitation, 4).ToStdString());
        double ch1 = 0, ch2 = 0;
        if (idToH1 != _hlws1.end())
        {
            auto cidToCH1 = idToH1->second.find(cid);
            if (cidToCH1 != idToH1->second.end())
            {
                ch1 = cidToCH1->second;
            }
        }
        if (idToH2 != _hlws2.end())
        {
            auto cidToCH2 = idToH2->second.find(cid);
            if (cidToCH2 != idToH2->second.end())
            {
                ch2 = cidToCH2->second;
            }
        }

        ListCtrlCitations->SetItemBackgroundColour(idxCitation, getColor(idxColor1,idxColor2,ch1,ch2));
        ListCtrlCitations->RefreshItem(idxCitation);
    }
}

void WESTSeerFrame::highlightCandidates()
{
    if (ChoiceScope->GetSelection() <0)
        return;

    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _displayThread = new std::thread(
        [this]
        {
            int idxColor1 = RadioBox1->GetSelection();
            int idxColor2 = RadioBox2->GetSelection();
            int numItems = ListCtrlPublications->GetItemCount();
            for (int idxItem = 0; idxItem < numItems; idxItem++)
            {
                uint64_t id = _ids[idxItem];

                double h1 = 0, h2 = 0;
                auto idToH1 = _hlws1.find(id);
                if (idToH1 != _hlws1.end())
                {
                    double sum1 = idToH1->second.size(), sum2 = 0;
                    for (auto cidToH1 : idToH1->second)
                    {
                        sum2 += cidToH1.second;
                    }
                    if (sum1 > 0 && sum2 > 0)
                        h1 = sum2 / sum1;
                    if (h1 > 1)
                        h1 = 1;
                }
                auto idToH2 = _hlws2.find(id);
                if (idToH2 != _hlws2.end())
                {
                    double sum1 = idToH2->second.size(), sum2 = 0;
                    for (auto cidToH2 : idToH2->second)
                    {
                        sum2 += cidToH2.second;
                    }
                    if (sum1 > 0 && sum2 > 0)
                        h2 = sum2 / sum1;
                    if (h2 > 1)
                        h2 = 1;
                }

                ListCtrlPublications->SetItemBackgroundColour(idxItem, getColor(idxColor1,idxColor2,h1,h2));
                ListCtrlPublications->RefreshItem(idxItem);
            }

            int idxPubSelection = (int)ListCtrlPublications->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (idxPubSelection >= 0 && idxPubSelection < (int) _ids.size())
            {
                uint64_t id = std::stoull(ListCtrlPublications->GetItemText(idxPubSelection, 4).ToStdString());
                highlightCitations(id, idxColor1, idxColor2);
            }
        }
    );
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
    showCandidates();
}

void WESTSeerFrame::OnExploreModeSelected(wxCommandEvent& event)
{
    _exploreMode = true;
    showCandidates();
}

void WESTSeerFrame::OnTextModeSelected(wxCommandEvent& event)
{
    _exploreMode = false;
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

void WESTSeerFrame::OnMenuViewDownloadsSelected(wxCommandEvent& event)
{
    DownloadsDialog dlg(this);
    dlg.ShowModal();
}

void WESTSeerFrame::OnMenuViewSchedulesSelected(wxCommandEvent& event)
{
    SchedulesDialog dlg(this);
    dlg.ShowModal();
}

void WESTSeerFrame::OnMenuAddScopeSelected(wxCommandEvent& event)
{
    // pop up an OpenAlexImportDialog
    OpenAlexImportDialog dlg(this);
    dlg.addScope();
}

void WESTSeerFrame::OnTimer1Trigger(wxTimerEvent& event)
{
    bool paused = false;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
    size_t numDone = 0;
    std::string latestStatus;
    for (size_t i = 0; i < schedule.size(); i++)
    {
        if (schedule[i].second == "Done")
            numDone++;
        else if (schedule[i].second != "Pending")
            latestStatus = schedule[i].second;
    }
    StatusBar1->SetStatusText(wxString::Format("#Sched: %d", (int)schedule.size()), 0);
    StatusBar1->SetStatusText(wxString::Format("#Done: %d", (int)numDone), 1);
    StatusBar1->SetStatusText(("Scope Progress:" + latestStatus).c_str(), 2);
    StatusBar1->SetStatusText(wxString::Format("#Read: %d", (int)CallbackData::getNumDataRead()),3);
    StatusBar1->SetStatusText(wxString::Format("Size Read: %d M", (int)(CallbackData::getSizeRead() >> 20)),4);
    StatusBar1->SetStatusText(wxString::Format("#Write: %d", (int)CallbackData::getNumDataWrite()),5);
    StatusBar1->SetStatusText(wxString::Format("Size Write: %d M", (int)(CallbackData::getSizeWrite() >> 20)),6);
    StatusBar1->SetStatusText(wxString::Format("#Receive: %d", (int)OpenAlex::numDownloaded()),7);
    StatusBar1->SetStatusText(wxString::Format("Size Receive: %d M", (int)(OpenAlex::sizeDownloaded() >> 20)),8);
}

void WESTSeerFrame::OnChoiceHL1Select(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _hlws1.clear();
    int idxSelection = ChoiceHL1->GetSelection();
    if (idxSelection <0 || idxSelection >= _numTermGroups + _numBitermGroups)
    {
        return;
    }

    if (idxSelection < _numTermGroups)
    {
        for (auto idToCHL = _termHLs.begin(); idToCHL != _termHLs.end(); idToCHL++)
        {
            std::map<uint64_t, double> chls;
            for (auto cidToHL = idToCHL->second.begin(); cidToHL != idToCHL->second.end(); cidToHL++)
            {
                chls[cidToHL->first] = cidToHL->second[idxSelection];
            }
            _hlws1[idToCHL->first] = chls;
        }
    }
    else
    {
        for (auto idToCHL = _bitermHLs.begin(); idToCHL != _bitermHLs.end(); idToCHL++)
        {
            std::map<uint64_t, double> chls;
            for (auto cidToHL = idToCHL->second.begin(); cidToHL != idToCHL->second.end(); cidToHL++)
            {
                chls[cidToHL->first] = cidToHL->second[idxSelection - _numTermGroups];
            }
            _hlws1[idToCHL->first] = chls;
        }
    }

    highlightCandidates();
}

void WESTSeerFrame::OnChoiceHL2Select(wxCommandEvent& event)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _hlws2.clear();
    int idxSelection = ChoiceHL2->GetSelection();
    if (idxSelection <0 || idxSelection >= _numTermGroups + _numBitermGroups)
    {
        return;
    }

    if (idxSelection < _numTermGroups)
    {
        for (auto idToCHL = _termHLs.begin(); idToCHL != _termHLs.end(); idToCHL++)
        {
            std::map<uint64_t, double> chls;
            for (auto cidToHL = idToCHL->second.begin(); cidToHL != idToCHL->second.end(); cidToHL++)
            {
                chls[cidToHL->first] = cidToHL->second[idxSelection];
            }
            _hlws2[idToCHL->first] = chls;
        }
    }
    else
    {
        for (auto idToCHL = _bitermHLs.begin(); idToCHL != _bitermHLs.end(); idToCHL++)
        {
            std::map<uint64_t, double> chls;
            for (auto cidToHL = idToCHL->second.begin(); cidToHL != idToCHL->second.end(); cidToHL++)
            {
                chls[cidToHL->first] = cidToHL->second[idxSelection - _numTermGroups];
            }
            _hlws2[idToCHL->first] = chls;
        }
    }

    highlightCandidates();
}

void WESTSeerFrame::OnListCtrlPublicationsItemRClick(wxListEvent& event)
{
    int idxItem = (int)ListCtrlPublications->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxItem < 0 || idxItem >= (int)ListCtrlPublications->GetItemCount())
        return;
    uint64_t id = std::stoull(ListCtrlPublications->GetItemText(idxItem, 4).ToStdString());
    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    ResearchScope scope(path, kws);

    Publication pub = scope.getPublication(id);
    wxMessageBox(pub.abstract(), "Abstract of Publication");
}

void WESTSeerFrame::OnListCtrlCitationsItemRClick(wxListEvent& event)
{
    int idxItem = (int)ListCtrlCitations->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxItem < 0 || idxItem >= (int)ListCtrlCitations->GetItemCount())
        return;
    uint64_t id = std::stoull(ListCtrlCitations->GetItemText(idxItem, 4).ToStdString());
    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    ResearchScope scope(path, kws);

    Publication pub = scope.getPublication(id);
    wxMessageBox(pub.abstract(), "Abstract of Citation");
}

void WESTSeerFrame::OnButtonRefreshClick(wxCommandEvent& event)
{
    clearCandidates();
    ChoiceScope->Clear();
    GeneralConfig config;
    std::vector<std::string> scopes = MetricModel::getScopesWithMetrics(config.getDatabase());
    for (std::string scope: scopes)
    {
        ChoiceScope->Append(scope);
    }
}

void WESTSeerFrame::OnRadioBox1Select(wxCommandEvent& event)
{
    highlightCandidates();
}

void WESTSeerFrame::OnRadioBox2Select(wxCommandEvent& event)
{
    highlightCandidates();
}
