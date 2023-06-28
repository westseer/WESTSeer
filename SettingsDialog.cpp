#include "SettingsDialog.h"
#include <EmailValidator.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <GeneralConfig.h>

//(*InternalHeaders(SettingsDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(SettingsDialog)
const long SettingsDialog::ID_STATICTEXT1 = wxNewId();
const long SettingsDialog::ID_TEXTCTRL1 = wxNewId();
const long SettingsDialog::ID_BUTTON1 = wxNewId();
const long SettingsDialog::ID_STATICTEXT2 = wxNewId();
const long SettingsDialog::ID_TEXTCTRL2 = wxNewId();
const long SettingsDialog::ID_BUTTON2 = wxNewId();
const long SettingsDialog::ID_STATICTEXT4 = wxNewId();
const long SettingsDialog::ID_SPINCTRL2 = wxNewId();
const long SettingsDialog::ID_BUTTON6 = wxNewId();
const long SettingsDialog::ID_STATICTEXT5 = wxNewId();
const long SettingsDialog::ID_SPINCTRLDOUBLE1 = wxNewId();
const long SettingsDialog::ID_BUTTON7 = wxNewId();
const long SettingsDialog::ID_STATICTEXT6 = wxNewId();
const long SettingsDialog::ID_SPINCTRL3 = wxNewId();
const long SettingsDialog::ID_BUTTON8 = wxNewId();
const long SettingsDialog::ID_PANEL1 = wxNewId();
const long SettingsDialog::ID_PANEL2 = wxNewId();
const long SettingsDialog::ID_STATICTEXT7 = wxNewId();
const long SettingsDialog::ID_TEXTCTRL3 = wxNewId();
const long SettingsDialog::ID_BUTTON9 = wxNewId();
const long SettingsDialog::ID_PANEL3 = wxNewId();
const long SettingsDialog::ID_NOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SettingsDialog,wxDialog)
	//(*EventTable(SettingsDialog)
	//*)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(SettingsDialog)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer4;

	Create(parent, id, _("WESTSeer Settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
	PanelGeneral = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText1 = new wxStaticText(PanelGeneral, ID_STATICTEXT1, _("Email:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxEXPAND, 5);
	TextCtrlEmail = new wxTextCtrl(PanelGeneral, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer2->Add(TextCtrlEmail, 1, wxALL|wxEXPAND, 5);
	ButtonValidateEmail = new wxButton(PanelGeneral, ID_BUTTON1, _("Validate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(ButtonValidateEmail, 1, wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(PanelGeneral, ID_STATICTEXT2, _("Path of Database:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxEXPAND, 5);
	TextCtrlDatabase = new wxTextCtrl(PanelGeneral, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer2->Add(TextCtrlDatabase, 1, wxALL|wxEXPAND, 5);
	ButtonCustomiseDatabase = new wxButton(PanelGeneral, ID_BUTTON2, _("Customise"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(ButtonCustomiseDatabase, 1, wxALL|wxEXPAND, 5);
	StaticText4 = new wxStaticText(PanelGeneral, ID_STATICTEXT4, _("#Biterms for Topic Description:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer2->Add(StaticText4, 1, wxALL|wxEXPAND, 5);
	SpinCtrlBiterms = new wxSpinCtrl(PanelGeneral, ID_SPINCTRL2, _T("5"), wxDefaultPosition, wxDefaultSize, 0, 3, 10, 5, _T("ID_SPINCTRL2"));
	SpinCtrlBiterms->SetValue(_T("5"));
	FlexGridSizer2->Add(SpinCtrlBiterms, 1, wxALL|wxEXPAND, 5);
	ButtonResetBiterms = new wxButton(PanelGeneral, ID_BUTTON6, _("Reset"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
	FlexGridSizer2->Add(ButtonResetBiterms, 1, wxALL|wxEXPAND, 5);
	StaticText5 = new wxStaticText(PanelGeneral, ID_STATICTEXT5, _("Relative TF-IRDF Threshold:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer2->Add(StaticText5, 1, wxALL|wxEXPAND, 5);
	SpinCtrlTFIRDF = new wxSpinCtrlDouble(PanelGeneral, ID_SPINCTRLDOUBLE1, _T("0.3"), wxDefaultPosition, wxDefaultSize, 0, 0.05, 0.5, 0.05, 0.05, _T("ID_SPINCTRLDOUBLE1"));
	SpinCtrlTFIRDF->SetValue(_T("0.3"));
	FlexGridSizer2->Add(SpinCtrlTFIRDF, 1, wxALL|wxEXPAND, 5);
	ButtonResetTFIRDF = new wxButton(PanelGeneral, ID_BUTTON7, _("Reset"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
	FlexGridSizer2->Add(ButtonResetTFIRDF, 1, wxALL|wxEXPAND, 5);
	StaticText6 = new wxStaticText(PanelGeneral, ID_STATICTEXT6, _("Citation Threshold:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer2->Add(StaticText6, 1, wxALL|wxEXPAND, 5);
	SpinCtrlCitations = new wxSpinCtrl(PanelGeneral, ID_SPINCTRL3, _T("20"), wxDefaultPosition, wxDefaultSize, 0, 10, 100, 20, _T("ID_SPINCTRL3"));
	SpinCtrlCitations->SetValue(_T("20"));
	FlexGridSizer2->Add(SpinCtrlCitations, 1, wxALL|wxEXPAND, 5);
	ButtonResetCitations = new wxButton(PanelGeneral, ID_BUTTON8, _("Reset"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
	FlexGridSizer2->Add(ButtonResetCitations, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	PanelGeneral->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(PanelGeneral);
	FlexGridSizer2->SetSizeHints(PanelGeneral);
	PanelNNP = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	PanelNNP->SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(PanelNNP);
	FlexGridSizer3->SetSizeHints(PanelNNP);
	PanelLLM = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer4->AddGrowableCol(1);
	StaticText7 = new wxStaticText(PanelLLM, ID_STATICTEXT7, _("Model File:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer4->Add(StaticText7, 1, wxALL|wxEXPAND, 5);
	TextCtrl3 = new wxTextCtrl(PanelLLM, ID_TEXTCTRL3, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	FlexGridSizer4->Add(TextCtrl3, 1, wxALL|wxEXPAND, 5);
	Button7 = new wxButton(PanelLLM, ID_BUTTON9, _("Customise"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
	FlexGridSizer4->Add(Button7, 1, wxALL|wxEXPAND, 5);
	PanelLLM->SetSizer(FlexGridSizer4);
	FlexGridSizer4->Fit(PanelLLM);
	FlexGridSizer4->SetSizeHints(PanelLLM);
	Notebook1->AddPage(PanelGeneral, _("General"), false);
	Notebook1->AddPage(PanelNNP, _("Neural Network for Prediction"), false);
	Notebook1->AddPage(PanelLLM, _("Large Language Model"), false);
	FlexGridSizer1->Add(Notebook1, 1, wxALL|wxEXPAND, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ButtonOK = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
	BoxSizer1->Add(ButtonOK, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	BoxSizer1->Add(ButtonCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonValidateEmailClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonCustomiseDatabaseClick);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&SettingsDialog::OnSpinCtrlBitermsChange);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonResetBitermsClick);
	Connect(ID_SPINCTRLDOUBLE1,wxEVT_SPINCTRLDOUBLE,(wxObjectEventFunction)&SettingsDialog::OnSpinCtrlTFIRDFChange);
	Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonResetTFIRDFClick);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&SettingsDialog::OnSpinCtrlCitationsChange);
	Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonResetCitationsClick);
	Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&SettingsDialog::OnNotebook1PageChanged);
	Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonOKClick);
	Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SettingsDialog::OnButtonCancelClick);
	//*)

	GeneralConfig config;
	TextCtrlEmail->SetValue(config.getEmail());
	TextCtrlDatabase->SetValue(config.getDatabase());
	SpinCtrlBiterms->SetValue(config.getBiterms());
	SpinCtrlTFIRDF->SetValue(config.getTfirdf());
	SpinCtrlCitations->SetValue(config.getCitations());
}

SettingsDialog::~SettingsDialog()
{
	//(*Destroy(SettingsDialog)
	//*)
}


void SettingsDialog::OnNotebook1PageChanged(wxNotebookEvent& event)
{
}

void SettingsDialog::OnButtonValidateEmailClick(wxCommandEvent& event)
{
    wxString email = TextCtrlEmail->GetValue();
    EmailValidator validator(email.ToStdString());
    if (!validator.valid())
    {
        wxMessageBox("Invalid email address.");
        TextCtrlEmail->SetValue("");
    }
}

void SettingsDialog::OnButtonCustomiseDatabaseClick(wxCommandEvent& event)
{
    GeneralConfig config;
    wxFileName fileName(config.getDatabase());
    wxFileDialog openFileDialog(this, _("Open database file"), fileName.GetPath(), fileName.GetName(), "SQLite files (*.sqlite)|*.sqlite", wxFD_OPEN);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;
    TextCtrlDatabase->SetValue(openFileDialog.GetPath());
}

void SettingsDialog::OnButtonResetBitermsClick(wxCommandEvent& event)
{
    SpinCtrlBiterms->SetValue(5);
}

void SettingsDialog::OnButtonResetTFIRDFClick(wxCommandEvent& event)
{
    SpinCtrlTFIRDF->SetValue(0.3);
}

void SettingsDialog::OnButtonResetCitationsClick(wxCommandEvent& event)
{
    SpinCtrlCitations->SetValue(20);
}

void SettingsDialog::OnSpinCtrlObYearsChange(wxSpinEvent& event)
{
}

void SettingsDialog::OnSpinCtrlBitermsChange(wxSpinEvent& event)
{
}

void SettingsDialog::OnSpinCtrlTFIRDFChange(wxSpinDoubleEvent& event)
{
}

void SettingsDialog::OnSpinCtrlCitationsChange(wxSpinEvent& event)
{
}

void SettingsDialog::OnButtonOKClick(wxCommandEvent& event)
{
    wxString email = TextCtrlEmail->GetValue();
    EmailValidator validator(email.ToStdString());
    if (!validator.valid())
    {
        wxMessageBox("Invalid email address.");
        return;
    }

    GeneralConfig config;
    config.setEmail(TextCtrlEmail->GetValue().ToStdString());
    config.setDatabase(TextCtrlDatabase->GetValue().ToStdString());
    config.setBiterms(SpinCtrlBiterms->GetValue());
    config.setTfirdf(SpinCtrlTFIRDF->GetValue());
    config.setCitations(SpinCtrlCitations->GetValue());
    EndModal(wxID_OK);
    Close();
}

void SettingsDialog::OnButtonCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
    Close();
}
