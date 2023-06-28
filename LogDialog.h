#ifndef LOGDIALOG_H
#define LOGDIALOG_H

//(*Headers(LogDialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class LogDialog: public wxDialog
{
	public:

		LogDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~LogDialog();

		//(*Declarations(LogDialog)
		wxButton* ButtonClose;
		wxButton* ButtonExport;
		wxButton* ButtonFind;
		wxListCtrl* ListCtrlLog;
		wxStaticText* StaticText1;
		wxTextCtrl* TextCtrlSearch;
		//*)

	protected:

		//(*Identifiers(LogDialog)
		static const long ID_LISTCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON3;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(LogDialog)
		void OnInit(wxInitDialogEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
