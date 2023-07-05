#include "FinishedScopeDialog.h"
#include "WESTSeerApp.h"
#include <GeneralConfig.h>
#include <StringProcessing.h>

//(*InternalHeaders(FinishedScopeDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(FinishedScopeDialog)
const long FinishedScopeDialog::ID_STATICTEXT1 = wxNewId();
const long FinishedScopeDialog::ID_CHOICE1 = wxNewId();
const long FinishedScopeDialog::ID_STATICTEXT2 = wxNewId();
const long FinishedScopeDialog::ID_RADIOBOX1 = wxNewId();
const long FinishedScopeDialog::ID_LISTCTRL1 = wxNewId();
const long FinishedScopeDialog::ID_LISTCTRL2 = wxNewId();
const long FinishedScopeDialog::ID_LISTCTRL3 = wxNewId();
const long FinishedScopeDialog::ID_TEXTCTRL1 = wxNewId();
const long FinishedScopeDialog::ID_TEXTCTRL2 = wxNewId();
const long FinishedScopeDialog::ID_TEXTCTRL3 = wxNewId();
const long FinishedScopeDialog::ID_TEXTCTRL4 = wxNewId();
const long FinishedScopeDialog::ID_NOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FinishedScopeDialog,wxDialog)
	//(*EventTable(FinishedScopeDialog)
	//*)
END_EVENT_TABLE()

FinishedScopeDialog::FinishedScopeDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(FinishedScopeDialog)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;

	Create(parent, id, _("Finished Scope Inspector"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxSize(800,600));
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer1->AddGrowableRow(2);
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Scope:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxEXPAND, 5);
	ChoiceScope = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	FlexGridSizer2->Add(ChoiceScope, 1, wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Mode:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Test"),
		_("Explore")
	};
	RadioBoxMode = new wxRadioBox(this, ID_RADIOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
	FlexGridSizer2->Add(RadioBoxMode, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
	ListCtrlPublications = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer1->Add(ListCtrlPublications, 1, wxALL|wxEXPAND, 5);
	NotebookInfo = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
	ListCtrlReferences = new wxListCtrl(NotebookInfo, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL2"));
	ListCtrlCitations = new wxListCtrl(NotebookInfo, ID_LISTCTRL3, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL3"));
	TextCtrlAbstract = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrlTopic = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrlPrediction = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextCtrlVerification = new wxTextCtrl(NotebookInfo, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	NotebookInfo->AddPage(ListCtrlReferences, _("References"), false);
	NotebookInfo->AddPage(ListCtrlCitations, _("Citations"), false);
	NotebookInfo->AddPage(TextCtrlAbstract, _("Abstract"), false);
	NotebookInfo->AddPage(TextCtrlTopic, _("Topic"), false);
	NotebookInfo->AddPage(TextCtrlPrediction, _("Prediction Summary"), false);
	NotebookInfo->AddPage(TextCtrlVerification, _("Verification Summary"), false);
	FlexGridSizer1->Add(NotebookInfo, 1, wxALL|wxEXPAND, 5);
	SetSizer(FlexGridSizer1);
	SetSizer(FlexGridSizer1);
	Layout();

	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&FinishedScopeDialog::OnChoiceScopeSelect);
	Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&FinishedScopeDialog::OnRadioBoxModeSelect);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&FinishedScopeDialog::OnListCtrlPublicationsItemSelect);
	//*)

	_exploreMode = false;
    _displayThread = NULL;
    GeneralConfig config;
    std::vector<std::string> scopes = MetricModel::getScopesWithMetrics(config.getDatabase());
    for (std::string scope: scopes)
    {
        ChoiceScope->Append(scope);
    }

    _exploreMode = (RadioBoxMode->GetSelection() != 0);
}

FinishedScopeDialog::~FinishedScopeDialog()
{
	//(*Destroy(FinishedScopeDialog)
	//*)
}

void FinishedScopeDialog::showCandidate(uint64_t id)
{
    if (_displayThread != NULL)
    {
        _displayThread->join();
        delete _displayThread;
        _displayThread = NULL;
    }

    _displayThread = new std::thread([this, id]{
        ListCtrlPublications->Disable();
        NotebookInfo->Disable();
        SetTitle("Finished Scope Inspector (0%)");
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
        SetTitle("Finished Scope Inspector (20%)");

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
        SetTitle("Finished Scope Inspector (60%)");

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
        SetTitle("Finished Scope Inspector (80%)");

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
        SetTitle("Finished Scope Inspector (100%)");
    });
}

void FinishedScopeDialog::showCandidates()
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
        ListCtrlPublications->ClearAll();
        ListCtrlReferences->ClearAll();
        ListCtrlCitations->ClearAll();

        SetTitle("Finished Scope Inspector (0%)");

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
        SetTitle("Finished Scope Inspector (10%)");

        if (!TopicIdentification::load(keywords, ye, &_topics))
        {
            logError("cannot load topics");
            return;
        }
        SetTitle("Finished Scope Inspector (20%)");

        if (!MetricModel::load(keywords, ye, &_scores))
        {
            logError("cannot load scores");
            return;
        }
        SetTitle("Finished Scope Inspector (30%)");

        GeneralConfig config;
        std::string path = config.getDatabase();
        std::string kws = ChoiceScope->GetString(ChoiceScope->GetSelection()).ToStdString();
        ResearchScope scope(path, kws);
        std::vector<uint64_t> ids;
        std::vector<int> pRanks, vRanks;
        if (CandidateIdentification::load(keywords, ye, &ids))
        {
            SetTitle("Finished Scope Inspector (35%)");

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
            SetTitle("Finished Scope Inspector (40%)");

            std::vector<Publication> pubs = scope.getPublications(_ids);
            std::map<uint64_t,Publication> pubMap;
            for (Publication pub: pubs)
            {
                int progress0 = 40 + 50 * pubMap.size() / pubs.size();
                pubMap[pub.id()] = pub;
                int progress1 = 40 + 50 * pubMap.size() / pubs.size();
                if (progress1 > progress0)
                {
                    wxString s = wxString::Format("Finished Scope Inspector (%d)", progress1);
                    SetTitle(s);
                }
            }
            SetTitle("Finished Scope Inspector (90%)");

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
                    wxString s = wxString::Format("Finished Scope Inspector (%d\%)", progress1);
                    SetTitle(s);
                }
            }
        }

        ListCtrlPublications->Enable();
        NotebookInfo->Enable();
    });
}

void FinishedScopeDialog::OnRadioBoxModeSelect(wxCommandEvent& event)
{
    _exploreMode = (RadioBoxMode->GetSelection() != 0);
    showCandidates();
}

void FinishedScopeDialog::OnChoiceScopeSelect(wxCommandEvent& event)
{
    showCandidates();
}

void FinishedScopeDialog::OnListCtrlPublicationsItemSelect(wxListEvent& event)
{
    int idxItem = (int)ListCtrlPublications->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxItem >= 0 && idxItem < (int) _ids.size())
        showCandidate(_ids[idxItem]);
}
