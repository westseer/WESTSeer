#include "SQLDialog.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <StringProcessing.h>
#include <sqlite3.h>
#include <time.h>
#include <GeneralConfig.h>
#include <wxFFileLog.h>
#include <CallbackData.h>

//(*InternalHeaders(SQLDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(SQLDialog)
const long SQLDialog::ID_STATICTEXT1 = wxNewId();
const long SQLDialog::ID_TEXTCTRL1 = wxNewId();
const long SQLDialog::ID_BUTTON1 = wxNewId();
const long SQLDialog::ID_LISTCTRL1 = wxNewId();
const long SQLDialog::ID_STATICTEXT2 = wxNewId();
const long SQLDialog::ID_TEXTCTRL2 = wxNewId();
const long SQLDialog::ID_BUTTON2 = wxNewId();
const long SQLDialog::ID_BUTTON3 = wxNewId();
const long SQLDialog::ID_BUTTON4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SQLDialog,wxDialog)
	//(*EventTable(SQLDialog)
	//*)
END_EVENT_TABLE()

SQLDialog::SQLDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(SQLDialog)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;

	Create(parent, id, _("Application Database"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxSize(800,600));
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("SQL:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxEXPAND, 5);
	TextCtrlSQL = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer2->Add(TextCtrlSQL, 1, wxALL|wxEXPAND, 5);
	ButtonExecute = new wxButton(this, ID_BUTTON1, _("Execute"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(ButtonExecute, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
	ListCtrlResults = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer1->Add(ListCtrlResults, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Search:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer3->Add(StaticText2, 1, wxALL|wxEXPAND, 5);
	TextCtrlSearch = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer3->Add(TextCtrlSearch, 1, wxALL|wxEXPAND, 5);
	ButtonFind = new wxButton(this, ID_BUTTON2, _("Find"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer3->Add(ButtonFind, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(FlexGridSizer3, 1, wxALL|wxEXPAND, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ButtonSave = new wxButton(this, ID_BUTTON3, _("Save Results"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer1->Add(ButtonSave, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonClose = new wxButton(this, ID_BUTTON4, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	BoxSizer1->Add(ButtonClose, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	SetSizer(FlexGridSizer1);
	Layout();

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SQLDialog::OnButtonExecuteClick);
	//*)
}

SQLDialog::~SQLDialog()
{
	//(*Destroy(SQLDialog)
	//*)
}

void SQLDialog::OnButtonExecuteClick(wxCommandEvent& event)
{
    ListCtrlResults->ClearAll();
    ListCtrlResults->DeleteAllColumns();

    GeneralConfig config;

    sqlite3 *db = NULL;
    int rc = sqlite3_open(config.getDatabase().c_str(), &db);
    if (rc != SQLITE_OK)
    {
        ListCtrlResults->AppendColumn("error", wxLIST_FORMAT_LEFT, 600);
        wxString strError = wxString::Format("Cannot open database at location %d", config.getDatabase().c_str());
        logError(strError);
        ListCtrlResults->InsertItem(0, strError);
        return;
    }

    CallbackData data(1000);
    char *errorMessage = NULL;
    rc = sqlite3_exec(db, TextCtrlSQL->GetValue().ToAscii(), CallbackData::sqliteCallback, &data, &errorMessage);
    if (rc == SQLITE_OK || data.results.size() > 0)
    {
        if (data.results.size() > 0)
        {
            std::vector<std::string> columns;
            for (auto keyToValue: data.results[0])
            {
                columns.push_back(keyToValue.first);
                ListCtrlResults->AppendColumn(keyToValue.first);
            }
            if (columns.size() > 0)
            {
                for (auto result: data.results)
                {
                    long idxItem = ListCtrlResults->InsertItem(0, result[columns[0]]);
                    for (size_t i = 1; i < columns.size(); i++)
                    {
                        ListCtrlResults->SetItem(idxItem, i, result[columns[i]]);
                    }
                }
            }
        }
        else
        {
            ListCtrlResults->AppendColumn("Message", wxLIST_FORMAT_LEFT, 600);
            wxString strMessage("No results found");
            ListCtrlResults->InsertItem(0, strMessage);
        }
    }
    else
    {
        ListCtrlResults->AppendColumn("error", wxLIST_FORMAT_LEFT, 600);
        logError(errorMessage);
        ListCtrlResults->InsertItem(0, errorMessage);
    }
    sqlite3_close(db);
    return;
}
