#include "DownloadsDialog.h"
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <TermExtraction.h>
#include <TermTfIrdf.h>
#include <BitermDf.h>
#include <BitermWeight.h>
#include <CandidateIdentification.h>
#include <TopicIdentification.h>
#include <TimeSeriesExtraction.h>
#include <PredictionModel.h>
#include <MetricModel.h>
#include <GeneralConfig.h>
#include <wx/msgdlg.h>
#include "OpenAlexImportDialog.h"
#include "WESTSeerApp.h"

//(*InternalHeaders(DownloadsDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DownloadsDialog)
const long DownloadsDialog::ID_LISTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DownloadsDialog,wxDialog)
	//(*EventTable(DownloadsDialog)
	//*)
END_EVENT_TABLE()

DownloadsDialog::DownloadsDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DownloadsDialog)
	Create(parent, wxID_ANY, _("Downloads"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
	ListCtrlScope = new wxListCtrl(this, ID_LISTCTRL1, wxPoint(352,352), wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	ListCtrlScope->SetToolTip(_("Select a scope and press \"Del\" to delete the scope. \nSelect a scope and press \"-\" to remove one year of records in the scope.\nRemove some records of the scope to force an update of the scope."));

	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&DownloadsDialog::OnListCtrlScopeItemRClick);
	Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&DownloadsDialog::OnKeyDown);
	//*)

	int ye = WESTSeerApp::year();
	int yb = ye - 15;
	GeneralConfig config;
	std::vector<std::string> scopes = ResearchScope::getResearchScopes(config.getDatabase());

    // create list ctrl header
    ListCtrlScope->AppendColumn("Scope", wxLIST_FORMAT_LEFT, 600);
    ListCtrlScope->AppendColumn("Progress");
    ListCtrlScope->AppendColumn("Latest Year");

    // insert scopes into list
    for (long i = 0; i < (long)scopes.size(); i++)
    {
        ResearchScope scope(config.getDatabase(), scopes[i]);
        int progress = scope.getProgress(yb, ye);
        int y = scope.getLatestYear(yb, ye);
        ListCtrlScope->InsertItem(i, scopes[i].c_str());
        ListCtrlScope->SetItem(i, 1, wxString::Format("%d", progress));
        ListCtrlScope->SetItem(i, 2, wxString::Format("%d", y));
    }
}

DownloadsDialog::~DownloadsDialog()
{
	//(*Destroy(DownloadsDialog)
	//*)
}


void DownloadsDialog::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE)
    {
        removeScope();
    }
    else if (event.GetKeyCode() == '-')
    {
        removeOneYearFromScope();
    }
}

void DownloadsDialog::removeScope()
{
    if (AbstractTask::taskThread() != NULL)
    {
        wxMessageBox("There are scheduled tasks running. Please try later or pause schedule first.");
        return;
    }

    int idxScope = (int)ListCtrlScope->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxScope < 0 || idxScope >= ListCtrlScope->GetItemCount())
        return;

    wxString scopeName = ListCtrlScope->GetItemText(idxScope);
    int answer = wxMessageBox("Delete scope?", "Confirm", wxYES_NO | wxCANCEL, this);
    if (answer == wxYES)
    {
        ResearchScope::remove(scopeName.ToStdString());
        ListCtrlScope->DeleteItem(idxScope);
    }
}

void DownloadsDialog::removeOneYearFromScope()
{
    if (AbstractTask::taskThread() != NULL)
    {
        wxMessageBox("There are scheduled tasks running. Please try later or paus schedule first.");
        return;
    }

    int idxScope = (int)ListCtrlScope->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxScope < 0 || idxScope >= ListCtrlScope->GetItemCount())
        return;

    std::string scopeName = ListCtrlScope->GetItemText(idxScope).ToStdString();
    int answer = wxMessageBox("Remove one year from scope?", "Confirm", wxYES_NO | wxCANCEL, this);
    if (answer == wxYES)
    {
        GeneralConfig config;
        ResearchScope scope(config.getDatabase(), scopeName);
        int ye = WESTSeerApp::year();
        int yb = ye - 15;
        int y = scope.getLatestYear(yb, ye);
        if (y > 0)
        {
            scope.removeOneYear(y);
            TermExtraction::removeOneYear(scopeName, y);
            TermTfIrdf::removeOneYear(scopeName, y);
            BitermDf::removeOneYear(scopeName, y);
            BitermWeight::removeOneYear(scopeName, y);
            CandidateIdentification::removeOneYear(scopeName, ye);
            CandidateIdentification::removeOneYear(scopeName, ye + 5);
            TopicIdentification::removeOneYear(scopeName, ye);
            TopicIdentification::removeOneYear(scopeName, ye + 5);
            TimeSeriesExtraction::removeOneYear(scopeName, ye);
            TimeSeriesExtraction::removeOneYear(scopeName, ye + 5);
            PredictionModel::removeOneYear(scopeName, ye);
            PredictionModel::removeOneYear(scopeName, ye + 5);
            MetricModel::removeOneYear(scopeName, ye);
            MetricModel::removeOneYear(scopeName, ye + 5);
        }
    }
}

void DownloadsDialog::OnListCtrlScopeItemRClick(wxListEvent& event)
{
    wxMenu menu;
    menu.Append(ID_NEW_SCOPE, "New Scope");
 	menu.Append(ID_REMOVE_SCOPE, 	"Remove Scope");
 	menu.Append(ID_REMOVE_ONE_YEAR_FROM_SCOPE, 	"Remove One Year from Scope");
 	menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DownloadsDialog::OnPopupClick), NULL, this);
 	PopupMenu(&menu);

}

void DownloadsDialog::newScope()
{
    if (AbstractTask::taskThread() != NULL)
    {
        wxMessageBox("There are scheduled tasks running. Please try later or pause schedule first.");
        return;
    }

    OpenAlexImportDialog dlg(this, -1);
    if (dlg.ShowModal())
    {
        if (dlg._openAlex != NULL)
        {
            // find whether it is in the old choices
            std::string keywords = dlg._openAlex->scope().getKeywords();
            long idxKW = -1;
            for (long i = 0; i < ListCtrlScope->GetItemCount(); i++)
            {
                wxString scopeName = ListCtrlScope->GetItemText(i);
                if (scopeName.ToStdString() == keywords)
                {
                    idxKW = i;
                    break;
                }
            }
            if (idxKW < 0)
            {
                // This is a new scope. We should put it
            }
        }
    }
}

void DownloadsDialog::OnPopupClick(wxCommandEvent &event)
 {
 	switch(event.GetId()) {
 		case ID_NEW_SCOPE:
 		    {

 		    }
 			break;
 		case ID_REMOVE_SCOPE:
 		    removeScope();
 			break;
        case ID_REMOVE_ONE_YEAR_FROM_SCOPE:
            removeOneYearFromScope();
            break;
 	}
 }
