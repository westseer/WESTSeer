#include "AddScheduleDialog.h"

//(*InternalHeaders(AddScheduleDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(AddScheduleDialog)
const long AddScheduleDialog::ID_LISTCTRL1 = wxNewId();
const long AddScheduleDialog::ID_BUTTON1 = wxNewId();
const long AddScheduleDialog::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(AddScheduleDialog,wxDialog)
	//(*EventTable(AddScheduleDialog)
	//*)
END_EVENT_TABLE()

AddScheduleDialog::AddScheduleDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(AddScheduleDialog)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	ListCtrlScope = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer1->Add(ListCtrlScope, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableCol(2);
	ButtonOK = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(ButtonOK, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonCancel = new wxButton(this, ID_BUTTON2, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(ButtonCancel, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&AddScheduleDialog::OnListCtrlScopeItemSelect);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AddScheduleDialog::OnButtonOKClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AddScheduleDialog::OnButtonCancelClick);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&AddScheduleDialog::OnInit);
	//*)

	_idxScope = -1;
}

AddScheduleDialog::~AddScheduleDialog()
{
	//(*Destroy(AddScheduleDialog)
	//*)
}


void AddScheduleDialog::OnListCtrlScopeItemSelect(wxListEvent& event)
{
}

void AddScheduleDialog::OnButtonCancelClick(wxCommandEvent& event)
{
    _idxScope = -1;
    EndModal(wxID_OK);
    Close();
}

void AddScheduleDialog::OnButtonOKClick(wxCommandEvent& event)
{
    _idxScope = (int)ListCtrlScope->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    EndModal(wxID_CANCEL);
    Close();
}

void AddScheduleDialog::OnInit(wxInitDialogEvent& event)
{
    ListCtrlScope->AppendColumn("Scope", wxLIST_FORMAT_LEFT, 600);
	for (size_t i = 0; i < _scopeKWs.size(); i++)
    {
        ListCtrlScope->InsertItem(i, _scopeKWs[i].c_str());
    }
}
