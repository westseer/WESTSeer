#include "LogDialog.h"
#include "WESTSeerApp.h"
#include <GeneralConfig.h>
#include <fstream>


//(*InternalHeaders(LogDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(LogDialog)
const long LogDialog::ID_LISTCTRL1 = wxNewId();
const long LogDialog::ID_STATICTEXT1 = wxNewId();
const long LogDialog::ID_TEXTCTRL1 = wxNewId();
const long LogDialog::ID_BUTTON3 = wxNewId();
const long LogDialog::ID_BUTTON1 = wxNewId();
const long LogDialog::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(LogDialog,wxDialog)
	//(*EventTable(LogDialog)
	//*)
END_EVENT_TABLE()

LogDialog::LogDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(LogDialog)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;

	Create(parent, id, _("Application Log"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	ListCtrlLog = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer1->Add(ListCtrlLog, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Search:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlSearch = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer2->Add(TextCtrlSearch, 1, wxALL|wxEXPAND, 5);
	ButtonFind = new wxButton(this, ID_BUTTON3, _("Find"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	FlexGridSizer2->Add(ButtonFind, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ButtonExport = new wxButton(this, ID_BUTTON1, _("Export"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer1->Add(ButtonExport, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonClose = new wxButton(this, ID_BUTTON2, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer1->Add(ButtonClose, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&LogDialog::OnInit);
	//*)

	ListCtrlLog->AppendColumn("Record", wxLIST_FORMAT_LEFT, 600);
}

LogDialog::~LogDialog()
{
	//(*Destroy(LogDialog)
	//*)
}



void LogDialog::OnInit(wxInitDialogEvent& event)
{
    WESTSeerApp::FlushLog();
    std::string line;
    GeneralConfig config;
    std::ifstream logFile(config.getLogFile());
    while (std::getline(logFile, line))
    {
        ListCtrlLog->InsertItem(0, line);
    }
}
