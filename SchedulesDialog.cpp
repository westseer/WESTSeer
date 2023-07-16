#include "SchedulesDialog.h"
#include "OpenAlexImportDialog.h"
#include "AddScheduleDialog.h"
#include <AbstractTask.h>
#include <MetricModel.h>
#include <GeneralConfig.h>
#include <wx/msgdlg.h>
#include "WESTSeerApp.h"

//(*InternalHeaders(SchedulesDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(SchedulesDialog)
const long SchedulesDialog::ID_LISTCTRL1 = wxNewId();
const long SchedulesDialog::ID_BUTTON7 = wxNewId();
const long SchedulesDialog::ID_BUTTON5 = wxNewId();
const long SchedulesDialog::ID_BUTTON6 = wxNewId();
const long SchedulesDialog::ID_BUTTON8 = wxNewId();
const long SchedulesDialog::ID_BUTTON1 = wxNewId();
const long SchedulesDialog::ID_BUTTON2 = wxNewId();
const long SchedulesDialog::ID_BUTTON3 = wxNewId();
const long SchedulesDialog::ID_BUTTON4 = wxNewId();
const long SchedulesDialog::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SchedulesDialog,wxDialog)
	//(*EventTable(SchedulesDialog)
	//*)
END_EVENT_TABLE()

SchedulesDialog::SchedulesDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(SchedulesDialog)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, _("Schedules"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	ListCtrlSchedules = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer1->Add(ListCtrlSchedules, 1, wxALL|wxEXPAND, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ButtonNew = new wxButton(this, ID_BUTTON7, _("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
	BoxSizer1->Add(ButtonNew, 1, wxALL|wxEXPAND, 5);
	ButtonAdd = new wxButton(this, ID_BUTTON5, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
	BoxSizer1->Add(ButtonAdd, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonRemove = new wxButton(this, ID_BUTTON6, _("Remove"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
	BoxSizer1->Add(ButtonRemove, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonClear = new wxButton(this, ID_BUTTON8, _("Clear"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
	BoxSizer1->Add(ButtonClear, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonMoveUp = new wxButton(this, ID_BUTTON1, _("Up"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer1->Add(ButtonMoveUp, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtoMovedown = new wxButton(this, ID_BUTTON2, _("Down"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer1->Add(ButtoMovedown, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonPause = new wxButton(this, ID_BUTTON3, _("Pause"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer1->Add(ButtonPause, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonResume = new wxButton(this, ID_BUTTON4, _("Resume"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	BoxSizer1->Add(ButtonResume, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxEXPAND, 5);
	SetSizer(FlexGridSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	Timer1.Start(1000, false);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonNewClick);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonAddClick);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonRemoveClick);
	Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonClearClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonMoveUpClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtoMovedownClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonPauseClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SchedulesDialog::OnButtonResumeClick);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&SchedulesDialog::OnTimer1Trigger);
	//*)

	WESTSeerApp::_makingSchedule = true;
	ListCtrlSchedules->AppendColumn("Scope", wxLIST_FORMAT_LEFT, 600);
	ListCtrlSchedules->AppendColumn("Status");
	bool paused = true;
	std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
	for (long i = 0; i < (long)schedule.size(); i++)
    {
        ListCtrlSchedules->InsertItem(i, schedule[i].first);
        ListCtrlSchedules->SetItem(i, 1, schedule[i].second);
    }
}

SchedulesDialog::~SchedulesDialog()
{
	//(*Destroy(SchedulesDialog)
	//*)
	WESTSeerApp::_makingSchedule = false;
}


void SchedulesDialog::OnButtonAddClick(wxCommandEvent& event)
{
    GeneralConfig config;
    std::set<std::string> excludedScopes;

    // exclude scopes that are already in schedule
    {
        bool paused = false;
        std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
        for (size_t i = 0; i < schedule.size(); i++)
        {
            excludedScopes.insert(schedule[i].first);
        }
    }

    // exclude scopes that are already done
    {
        std::vector<std::string> doneScopes = MetricModel::getScopesWithMetrics(config.getDatabase());
        for (size_t i = 0; i < doneScopes.size(); i++)
        {
            excludedScopes.insert(doneScopes[i]);
        }
    }

    // find available scopes
    std::vector<std::string> availableScopes;
    {
        std::vector<std::string> scopes = ResearchScope::getResearchScopes(config.getDatabase());
        for (size_t i = 0; i < scopes.size(); i++)
        {
            if (excludedScopes.find(scopes[i]) == excludedScopes.end())
            {
                availableScopes.push_back(scopes[i]);
            }
        }
    }

    AddScheduleDialog dlg(this);
    dlg._scopeKWs = availableScopes;
    dlg.ShowModal();
    if (dlg._idxScope >= 0)
    {
        bool paused = true;
        std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
        std::pair<std::string,std::string> item(availableScopes[dlg._idxScope], "Pending");
        ListCtrlSchedules->InsertItem((int)schedule.size(), item.first.c_str());
        ListCtrlSchedules->SetItem((int)schedule.size(), 1, item.second.c_str());
        schedule.push_back(item);
        AbstractTask::saveSchedule(schedule, paused);
    }
}

void SchedulesDialog::OnButtonRemoveClick(wxCommandEvent& event)
{
    int idxSchedule = (int)ListCtrlSchedules->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxSchedule < 0 || idxSchedule >= ListCtrlSchedules->GetItemCount())
        return;

    int answer = wxMessageBox("Remove scheduled scope?", "Confirm", wxYES_NO | wxCANCEL, this);
    if (answer == wxYES)
    {
        bool paused = true;
        std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
        if (schedule[idxSchedule].second != "Done" && schedule[idxSchedule].second != "Pending")
        {
            if (AbstractTask::taskThread() != NULL)
            {
                AbstractTask::finalize();
            }
        }
        schedule.erase(schedule.begin() + idxSchedule);
        AbstractTask::saveSchedule(schedule, paused);
        ListCtrlSchedules->DeleteAllItems();
        for (long i = 0; i < (long)schedule.size(); i++)
        {
            ListCtrlSchedules->InsertItem(i, schedule[i].first);
            ListCtrlSchedules->SetItem(i, 1, schedule[i].second);
        }
    }
}

void SchedulesDialog::OnButtonMoveUpClick(wxCommandEvent& event)
{
    int idxSchedule = (int)ListCtrlSchedules->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxSchedule < 0 || idxSchedule >= ListCtrlSchedules->GetItemCount())
        return;

    if (idxSchedule == 0)
        return;

    bool paused = true;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
    if (schedule[idxSchedule].second == "Done" || schedule[idxSchedule - 1].second == "Done")
        return;

    if (schedule[idxSchedule - 1].second != "Pending")
    {
        wxMessageBox("Please pause before moving a running item.");
        return;
    }

    auto temp = schedule[idxSchedule - 1];
    schedule[idxSchedule - 1] = schedule[idxSchedule];
    schedule[idxSchedule] = temp;
    AbstractTask::saveSchedule(schedule, paused);

    ListCtrlSchedules->SetItem(idxSchedule, 0, schedule[idxSchedule].first);
    ListCtrlSchedules->SetItem(idxSchedule, 1, schedule[idxSchedule].second);
    ListCtrlSchedules->SetItem(idxSchedule - 1, 0, schedule[idxSchedule - 1].first);
    ListCtrlSchedules->SetItem(idxSchedule - 1, 1, schedule[idxSchedule - 1].second);
    ListCtrlSchedules->SetItemState(idxSchedule, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
    ListCtrlSchedules->SetItemState(idxSchedule - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    ListCtrlSchedules->RefreshItem(idxSchedule);
    ListCtrlSchedules->RefreshItem(idxSchedule - 1);

}

void SchedulesDialog::OnButtoMovedownClick(wxCommandEvent& event)
{
    int idxSchedule = (int)ListCtrlSchedules->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxSchedule < 0 || idxSchedule >= ListCtrlSchedules->GetItemCount())
        return;

    if (idxSchedule == ListCtrlSchedules->GetItemCount() - 1)
        return;

    bool paused = true;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
    if (schedule[idxSchedule].second == "Done" || schedule[idxSchedule + 1].second == "Done")
        return;

    if (schedule[idxSchedule].second != "Pending")
    {
        wxMessageBox("Please pause before moving a running item.");
        return;
    }

    auto temp = schedule[idxSchedule + 1];
    schedule[idxSchedule + 1] = schedule[idxSchedule];
    schedule[idxSchedule] = temp;
    AbstractTask::saveSchedule(schedule, paused);

    ListCtrlSchedules->SetItem(idxSchedule, 0, schedule[idxSchedule].first);
    ListCtrlSchedules->SetItem(idxSchedule, 1, schedule[idxSchedule].second);
    ListCtrlSchedules->SetItem(idxSchedule + 1, 0, schedule[idxSchedule + 1].first);
    ListCtrlSchedules->SetItem(idxSchedule + 1, 1, schedule[idxSchedule + 1].second);
    ListCtrlSchedules->SetItemState(idxSchedule, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
    ListCtrlSchedules->SetItemState(idxSchedule + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    ListCtrlSchedules->RefreshItem(idxSchedule);
    ListCtrlSchedules->RefreshItem(idxSchedule + 1);
}

void SchedulesDialog::OnButtonPauseClick(wxCommandEvent& event)
{
    if (AbstractTask::taskThread() == NULL)
        return;

    AbstractTask::cancel();
    AbstractTask::finalize();
    bool paused = false;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
    for (long i = 0; i < (long)schedule.size(); i++)
    {
        if (schedule[i].second != "Pending" && schedule[i].second != "Done")
            schedule[i].second = "Pending";
    }
    AbstractTask::saveSchedule(schedule, true);
    for (long i = 0; i < (long)schedule.size(); i++)
    {
        ListCtrlSchedules->SetItem(i, 1, schedule[i].second);
    }
    ListCtrlSchedules->RefreshItems(0, ListCtrlSchedules->GetItemCount() - 1);
}

void SchedulesDialog::OnButtonResumeClick(wxCommandEvent& event)
{
    if (AbstractTask::taskThread() != NULL)
        return;
    bool paused = true;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
    AbstractTask::saveSchedule(schedule, false);
}

void SchedulesDialog::OnTimer1Trigger(wxTimerEvent& event)
{
    bool paused = false;
    std::vector<std::pair<std::string,std::string>> schedule = AbstractTask::loadSchedule(&paused);
    if (!paused && (int)schedule.size() == ListCtrlSchedules->GetItemCount())
    {
        for (long i = 0; i < (long)schedule.size(); i++)
        {
            if (schedule[i].second != ListCtrlSchedules->GetItemText(i, 1).ToStdString())
            {
                ListCtrlSchedules->SetItem(i, 1, schedule[i].second.c_str());
                ListCtrlSchedules->RefreshItem(i);
            }
        }
    }
}

void SchedulesDialog::OnButtonNewClick(wxCommandEvent& event)
{
    OpenAlexImportDialog dlg(this, -1);
    std::string newScope = dlg.addScope();
    if (newScope != "")
    {
        int i = ListCtrlSchedules->GetItemCount();
        ListCtrlSchedules->InsertItem(i, newScope.c_str());
        ListCtrlSchedules->SetItem(i, 1, "Pending");
    }
}

void SchedulesDialog::OnButtonClearClick(wxCommandEvent& event)
{
    int answer = wxMessageBox("Remove all scheduled scopes?", "Confirm", wxYES_NO | wxCANCEL, this);
    if (answer == wxYES)
    {
        if (AbstractTask::taskThread() != NULL)
        {
            AbstractTask::finalize();
        }
        std::vector<std::pair<std::string,std::string>> schedule;
        AbstractTask::saveSchedule(schedule, true);
        ListCtrlSchedules->DeleteAllItems();
    }
}
