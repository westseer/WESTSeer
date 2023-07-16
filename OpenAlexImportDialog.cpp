#include "OpenAlexImportDialog.h"
#include <EmailValidator.h>
#include <GeneralConfig.h>
#include <wx/msgdlg.h>
#include <ResearchScope.h>

//(*InternalHeaders(OpenAlexImportDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(OpenAlexImportDialog)
const long OpenAlexImportDialog::ID_STATICTEXT2 = wxNewId();
const long OpenAlexImportDialog::ID_TEXTCTRL2 = wxNewId();
const long OpenAlexImportDialog::ID_BUTTON2 = wxNewId();
const long OpenAlexImportDialog::ID_STATICTEXT5 = wxNewId();
const long OpenAlexImportDialog::ID_GAUGE1 = wxNewId();
const long OpenAlexImportDialog::ID_STATICTEXT6 = wxNewId();
const long OpenAlexImportDialog::ID_STATICTEXT7 = wxNewId();
const long OpenAlexImportDialog::ID_CHOICE1 = wxNewId();
const long OpenAlexImportDialog::ID_LISTCTRL2 = wxNewId();
const long OpenAlexImportDialog::ID_STATICTEXT1 = wxNewId();
const long OpenAlexImportDialog::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OpenAlexImportDialog,wxDialog)
	//(*EventTable(OpenAlexImportDialog)
	//*)
END_EVENT_TABLE()

OpenAlexImportDialog::OpenAlexImportDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(OpenAlexImportDialog)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;

	Create(parent, wxID_ANY, _("Import Data from OpenAlex"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxBORDER_DOUBLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Keywords:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlKeywords = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(-1,150), wxTE_MULTILINE|wxTE_WORDWRAP|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrlKeywords->SetToolTip(_("Seperate synonyms in each group with commas. Seperate groups with semicolons."));
	FlexGridSizer2->Add(TextCtrlKeywords, 1, wxALL|wxEXPAND, 5);
	ButtonGetSamples = new wxButton(this, ID_BUTTON2, _("&Get Samples"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	ButtonGetSamples->Disable();
	FlexGridSizer2->Add(ButtonGetSamples, 1, wxALL|wxEXPAND, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Sampling Progress:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer2->Add(StaticText5, 1, wxALL|wxEXPAND, 5);
	GaugeSamplingProgress = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAUGE1"));
	FlexGridSizer2->Add(GaugeSamplingProgress, 1, wxALL|wxEXPAND, 5);
	StaticTextSamplingPrompt = new wxStaticText(this, ID_STATICTEXT6, _("0%"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer2->Add(StaticTextSamplingPrompt, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT7, _("Samples to Inspect:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer2->Add(StaticText6, 1, wxALL|wxEXPAND, 5);
	ChoiceKeywordPair = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	FlexGridSizer2->Add(ChoiceKeywordPair, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	ListCtrlSamples = new wxListCtrl(this, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL2"));
	BoxSizer2->Add(ListCtrlSamples, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableRow(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Abstract:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer3->Add(StaticText1, 1, wxALL|wxEXPAND, 5);
	TextCtrlSampleAbstract = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxVSCROLL, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrlSampleAbstract->Disable();
	FlexGridSizer3->Add(TextCtrlSampleAbstract, 1, wxALL|wxEXPAND, 5);
	BoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ButtonOK = new wxButton(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
	ButtonOK->Disable();
	BoxSizer1->Add(ButtonOK, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonCancel = new wxButton(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	BoxSizer1->Add(ButtonCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&OpenAlexImportDialog::OnTextCtrlKeywords1Text);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAlexImportDialog::OnButtonGetSamplesClick);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&OpenAlexImportDialog::OnChoiceKeywordPairSelect);
	Connect(ID_LISTCTRL2,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&OpenAlexImportDialog::OnListCtrlSamplesItemSelect);
	Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAlexImportDialog::OnButtonOKClick);
	Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAlexImportDialog::OnButtonCancelClick);
	//*)


	_openAlex = NULL;
	_progressReporter = new MyProgressReporter(this);

	ListCtrlSamples->AppendColumn("Year");
    ListCtrlSamples->AppendColumn("Title", wxLIST_FORMAT_LEFT, 600);
    ListCtrlSamples->AppendColumn("Authors");
    ListCtrlSamples->AppendColumn("Source");
    ListCtrlSamples->AppendColumn("ID");
}

OpenAlexImportDialog::~OpenAlexImportDialog()
{
	//(*Destroy(OpenAlexImportDialog)
	//*)

	delete _progressReporter;
}

void OpenAlexImportDialog::OnTextCtrlSampleAbstractText(wxCommandEvent& event)
{
    delete _openAlex;
}

void OpenAlexImportDialog::OnButtonGetSamplesClick(wxCommandEvent& event)
{
    if (_openAlex != NULL)
    {
        delete _openAlex;
        _openAlex = NULL;
    }
    TextCtrlKeywords->Disable();
    ButtonGetSamples->Disable();
    ButtonOK->Disable();
    ChoiceKeywordPair->Disable();
    ListCtrlSamples->Disable();
    ChoiceKeywordPair->Clear();
    _keywordPairs.clear();

    AbstractTask::setProgressReporter(_progressReporter);
    GeneralConfig config;
    _openAlex = new OpenAlex(config.getEmail(), config.getDatabase(),
                             TextCtrlKeywords->GetValue().ToStdString(), true);
    _openAlex->runAll();
}

void OpenAlexImportDialog::OnButtonOKClick(wxCommandEvent& event)
{
    AbstractTask::setProgressReporter(NULL);
    EndModal(wxID_OK);
    Close();
}

void OpenAlexImportDialog::OnButtonCancelClick(wxCommandEvent& event)
{
    AbstractTask::setProgressReporter(NULL);
    AbstractTask::cancel();
    if (_openAlex != NULL)
    {
        delete _openAlex;
        _openAlex = NULL;
    }
    EndModal(wxID_CANCEL);
    Close();
}

void OpenAlexImportDialog::OnChoiceKeywordPairSelect(wxCommandEvent& event)
{
    int idxKWPair = ChoiceKeywordPair->GetSelection();
    if (idxKWPair >= 0 && (size_t)idxKWPair < _keywordPairs.size())
    {
        showSamples(idxKWPair);
        TextCtrlSampleAbstract->ChangeValue("");
    }
}

void OpenAlexImportDialog::showSamples(int idxKWPair)
{
    if (idxKWPair < 0 || (size_t)idxKWPair >= _keywordPairs.size() || (size_t)idxKWPair >= _samples.size())
        return;
    ListCtrlSamples->DeleteAllItems();
    for (Publication &p: _samples[idxKWPair])
    {
        std::stringstream ssYear;
        ssYear << p.year();
        long idxItem = ListCtrlSamples->InsertItem(0, ssYear.str());
        ListCtrlSamples->SetItem(idxItem, 1, p.title());
        std::stringstream ssAuthors;
        const std::vector<wxString> &authors = p.authors();
        for (size_t i = 0; i < authors.size(); i++)
        {
            if (i > 0)
                ssAuthors << ", ";
            ssAuthors << authors[i];
        }
        ListCtrlSamples->SetItem(idxItem, 2, ssAuthors.str());
        ListCtrlSamples->SetItem(idxItem, 3, p.source());
        std::stringstream ssId;
        ssId <<p.id();
        ListCtrlSamples->SetItem(idxItem, 4, ssId.str());
    }
}

void OpenAlexImportDialog::showAbstract(int idxKWPair, int idxSample)
{
    if (idxKWPair < 0 || (size_t)idxKWPair >= _keywordPairs.size() || (size_t)idxKWPair >= _samples.size())
        return;
    if (idxSample < 0 || (size_t)idxSample >= _samples[idxKWPair].size())
        return;
    const wchar_t *ab = _samples[idxKWPair][idxSample].abstract();
    TextCtrlSampleAbstract->ChangeValue(ab);
}

void OpenAlexImportDialog::OnListCtrlSamplesItemSelect(wxListEvent& event)
{
    int idxKWPair = ChoiceKeywordPair->GetSelection();
    if (idxKWPair >= 0 && (size_t)idxKWPair < _keywordPairs.size())
    {
        int idxSample = (int)ListCtrlSamples->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        showAbstract(idxKWPair, idxSample);
    }
}

void OpenAlexImportDialog::OnTextCtrlEmailText(wxCommandEvent& event)
{
    ButtonGetSamples->Disable();
}

void OpenAlexImportDialog::OnTextCtrlKeywords2Text(wxCommandEvent& event)
{
    ButtonOK->Disable();
    if (TextCtrlKeywords->GetValue().size() > 0)
    {
        ButtonGetSamples->Enable();
    }
    else
    {
        ButtonGetSamples->Disable();
    }
}

void OpenAlexImportDialog::OnTextCtrlKeywords1Text(wxCommandEvent& event)
{
    ButtonOK->Disable();
    if (TextCtrlKeywords->GetValue().size() > 0)
    {
        ButtonGetSamples->Enable();
    }
    else
    {
        ButtonGetSamples->Disable();
    }
}

std::string OpenAlexImportDialog::addScope()
{
    if (AbstractTask::taskThread() != NULL)
    {
        wxMessageBox("Cannot add scope when schedule is running. Please pause the schedule or wait until it is done.");
        return "";
    }

    // pop up an OpenAlexImportDialog
    if (ShowModal())
    {
        if (_openAlex != NULL)
        {
            // find whether it is in the old choices
            GeneralConfig config;
            std::vector<std::string> scopes = ResearchScope::getResearchScopes(config.getDatabase());
            std::string keywords = _openAlex->scope().getKeywords();
            int idxKW = -1;
            for (unsigned int i = 0; i < scopes.size(); i++)
            {
                if (scopes[i] == keywords)
                {
                    idxKW = (int) i;
                    break;
                }
            }
            if (idxKW >= 0)
            {
                wxMessageBox("Scope " + scopes[idxKW] + " already exists.");
                return "";
            }
            else
            {
                bool paused = true;
                std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
                std::pair<std::string,std::string> item(keywords, "Pending");
                schedule.push_back(item);
                AbstractTask::saveSchedule(schedule, paused);
                return item.first;
            }
        }
    }
    return "";
}
