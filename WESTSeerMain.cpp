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
#include <GeneralConfig.h>
#include <wx/msgdlg.h>

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
const long WESTSeerFrame::ID_MENUITEM1 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM2 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM6 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM7 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM8 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM9 = wxNewId();
const long WESTSeerFrame::ID_MENUITEM10 = wxNewId();
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
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("&Explore"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem3);
    MenuItem4 = new wxMenuItem(Menu3, ID_MENUITEM2, _("&Test"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem4);
    MenuBar1->Append(Menu3, _("&Mode"));
    Menu4 = new wxMenu();
    MenuItemSQL = new wxMenuItem(Menu4, ID_MENUITEM6, _("&SQL"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItemSQL);
    MenuItemLog = new wxMenuItem(Menu4, ID_MENUITEM7, _("&Log"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItemLog);
    MenuItemForward = new wxMenuItem(Menu4, ID_MENUITEM8, _("&Recollect Data"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItemForward);
    MenuItemBackward = new wxMenuItem(Menu4, ID_MENUITEM9, _("&Analyse Again"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItemBackward);
    MenuItem5 = new wxMenuItem(Menu4, ID_MENUITEM10, _("&Back to Last Task"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem5);
    MenuBar1->Append(Menu4, _("&Debug"));
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
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnQuit);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnExploreModeSelected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&WESTSeerFrame::OnTextModeSelected);
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
    _exploreMode = true;
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
    if (_metricModel == NULL || _topicIdentification == NULL)
        return;

    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    ResearchScope scope(path, kws);
    Publication pub = scope.getPublication(id);
    int ye = _exploreMode ? WESTSeerApp::year() + 5 : WESTSeerApp::year();
    Publication me = scope.getPublication(id);
    std::pair<std::string,string> topic = scope.getTopic(id, ye, _topicIdentification);
    TextCtrlTopic->SetValue(topic.second);
    TextCtrlAbstract->SetValue(me.abstract());
    std::map<uint64_t, std::vector<double>> scores;
    if (_metricModel->load(ye, &scores))
    {
        auto idToScore = scores.find(id);
        if (idToScore != scores.end())
        {
            std::stringstream ss;
            ss << "predicted score = " << idToScore->second[0] << std::endl
                << "citations on topic (the last five values are predicted): " << std::endl;
            for (int i = 0; i < 15; i++)
            {
                ss << ye - 15 + i << ": " << idToScore->second[2 + i] << std::endl;
            }
            TextCtrlPrediction->SetValue(ss.str().c_str());
        }
        if (idToScore != scores.end() && !_exploreMode)
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
    std::vector<Publication> citations = scope.getCitations(id, ye);
    for (Publication pub: citations)
    {
        long row = ListCtrlCitations->InsertItem(0, wxString::Format("%d", pub.year()));
        ListCtrlCitations->SetItem(row, 1, pub.title().c_str());
        std::stringstream ssAuthors;
        std::vector<wxString> authors = pub.authors();
        for (size_t i = 0; i < authors.size(); i++)
        {
            if (i > 0)
                ssAuthors << ",";
            ssAuthors << authors[i].ToStdString();
        }
        ListCtrlCitations->SetItem(row, 2, ssAuthors.str().c_str());
        ListCtrlCitations->SetItem(row, 3, pub.source().c_str());
        ListCtrlCitations->SetItem(row, 4, wxString::Format("%llu", pub.id()));
    }

    ListCtrlReferences->ClearAll();
    ListCtrlReferences->AppendColumn("Year");
    ListCtrlReferences->AppendColumn("Title", wxLIST_FORMAT_LEFT, 600);
    ListCtrlReferences->AppendColumn("Authors");
    ListCtrlReferences->AppendColumn("Source");
    ListCtrlReferences->AppendColumn("ID");
    std::vector<Publication> references = scope.getReferences(id);
    for (Publication pub: references)
    {
        long row = ListCtrlReferences->InsertItem(0, wxString::Format("%d", pub.year()));
        ListCtrlReferences->SetItem(row, 1, pub.title().c_str());
        std::stringstream ssAuthors;
        std::vector<wxString> authors = pub.authors();
        for (size_t i = 0; i < authors.size(); i++)
        {
            if (i > 0)
                ssAuthors << ",";
            ssAuthors << authors[i].ToStdString();
        }
        ListCtrlReferences->SetItem(row, 2, ssAuthors.str().c_str());
        ListCtrlReferences->SetItem(row, 3, pub.source().c_str());
        ListCtrlReferences->SetItem(row, 4, wxString::Format("%llu", pub.id()));
    }
}

void WESTSeerFrame::showCandidates()
{
    if (_candidateIdentification == NULL || _metricModel == NULL)
        return;

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
    int ye = _exploreMode ? WESTSeerApp::year() + 5 : WESTSeerApp::year();
    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
    ResearchScope scope(path, kws);
    std::vector<uint64_t> ids;
    std::vector<int> pRanks, vRanks;
    std::map<uint64_t,std::vector<double>> scores;
    if (_candidateIdentification->load(ye, &ids) && _metricModel->load(ye, &scores))
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

                if (scores[ids[j]][0] > scores[ids[i]][0])
                    myPRank++;
                else if (scores[ids[j]][0] == scores[ids[i]][0] && j < i)
                    myPRank++;

                if (scores[ids[j]][1] > scores[ids[i]][1])
                    myVRank++;
                else if (scores[ids[j]][1] == scores[ids[i]][1] && j < i)
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

        std::vector<Publication> pubs = scope.getPublications(_ids);
        std::map<uint64_t,Publication> pubMap;
        for (Publication pub: pubs)
        {
            pubMap[pub.id()] = pub;
        }
        for (int i = (int)_ids.size() - 1; i >= 0 ; i--)
        {
            Publication pub = pubMap[_ids[i]];
            long row = ListCtrlPublications->InsertItem(0, wxString::Format("%d", pub.year()));
            ListCtrlPublications->SetItem(row, 1, pub.title().c_str());
            std::stringstream ssAuthors;
            std::vector<wxString> authors = pub.authors();
            for (size_t i = 0; i < authors.size(); i++)
            {
                if (i > 0)
                    ssAuthors << ",";
                ssAuthors << authors[i].ToStdString();
            }
            ListCtrlPublications->SetItem(row, 2, ssAuthors.str().c_str());
            ListCtrlPublications->SetItem(row, 3, pub.source().c_str());
            ListCtrlPublications->SetItem(row, 4, wxString::Format("%llu", pub.id()));
            double pScore = scores[pub.id()][0];
            double vScore = scores[pub.id()][1];
            ListCtrlPublications->SetItem(row, 5, wxString::Format("%lf", pScore));
            ListCtrlPublications->SetItem(row, 6, wxString::Format("%d", (int)i));
            if (!_exploreMode)
            {
                ListCtrlPublications->SetItem(row, 7, wxString::Format("%lf", vScore));
                ListCtrlPublications->SetItem(row, 8, wxString::Format("%d", _vRanks[i]));
            }
        }
    }
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
    ss2 << taskProgress << "%";
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
        _frame->ListCtrlPublications->Enable();
        _frame->NotebookInfo->Enable();
    }
    else
    {
        _frame->ButtonPause->Enable();
        _frame->ButtonResume->Disable();
        _frame->ChoiceScope->Disable();
        _frame->ButtonNew->Disable();
        _frame->ListCtrlPublications->Disable();
        _frame->NotebookInfo->Disable();
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
    clearCandidates();
    clearScope();
    AbstractTask::setProgressReporter(_progressReporter);
    GeneralConfig config;
    std::string path = config.getDatabase();
    std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
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
    _exploreMode = true;
    clearCandidates();
    showCandidates();
}

void WESTSeerFrame::OnTextModeSelected(wxCommandEvent& event)
{
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
