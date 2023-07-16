#ifndef ADDSCHEDULEDIALOG_H
#define ADDSCHEDULEDIALOG_H
#include <vector>
#include <string>

//(*Headers(AddScheduleDialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
//*)

class AddScheduleDialog: public wxDialog
{
	public:

		AddScheduleDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~AddScheduleDialog();

		//(*Declarations(AddScheduleDialog)
		wxButton* ButtonCancel;
		wxButton* ButtonOK;
		wxListCtrl* ListCtrlScope;
		//*)

		std::vector<std::string> _scopeKWs;
		int _idxScope;

	protected:

		//(*Identifiers(AddScheduleDialog)
		static const long ID_LISTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(AddScheduleDialog)
		void OnListCtrlScopeItemSelect(wxListEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonOKClick(wxCommandEvent& event);
		void OnInit(wxInitDialogEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
