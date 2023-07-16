#ifndef SCHEDULESDIALOG_H
#define SCHEDULESDIALOG_H

//(*Headers(SchedulesDialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/timer.h>
//*)

class SchedulesDialog: public wxDialog
{
	public:

		SchedulesDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SchedulesDialog();

		//(*Declarations(SchedulesDialog)
		wxButton* ButtoMovedown;
		wxButton* ButtonAdd;
		wxButton* ButtonClear;
		wxButton* ButtonMoveUp;
		wxButton* ButtonNew;
		wxButton* ButtonPause;
		wxButton* ButtonRemove;
		wxButton* ButtonResume;
		wxListCtrl* ListCtrlSchedules;
		wxTimer Timer1;
		//*)

	protected:

		//(*Identifiers(SchedulesDialog)
		static const long ID_LISTCTRL1;
		static const long ID_BUTTON7;
		static const long ID_BUTTON5;
		static const long ID_BUTTON6;
		static const long ID_BUTTON8;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_TIMER1;
		//*)

	private:

		//(*Handlers(SchedulesDialog)
		void OnButtonAddClick(wxCommandEvent& event);
		void OnButtonRemoveClick(wxCommandEvent& event);
		void OnButtonMoveUpClick(wxCommandEvent& event);
		void OnButtoMovedownClick(wxCommandEvent& event);
		void OnButtonPauseClick(wxCommandEvent& event);
		void OnButtonResumeClick(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void OnButtonNewClick(wxCommandEvent& event);
		void OnButtonClearClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
