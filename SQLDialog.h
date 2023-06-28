#ifndef SQLDIALOG_H
#define SQLDIALOG_H

//(*Headers(SQLDialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class SQLDialog: public wxDialog
{
	public:

		SQLDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SQLDialog();

		//(*Declarations(SQLDialog)
		wxButton* ButtonClose;
		wxButton* ButtonExecute;
		wxButton* ButtonFind;
		wxButton* ButtonSave;
		wxListCtrl* ListCtrlResults;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxTextCtrl* TextCtrlSQL;
		wxTextCtrl* TextCtrlSearch;
		//*)

	protected:

		//(*Identifiers(SQLDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_LISTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		//*)

	private:

		//(*Handlers(SQLDialog)
		void OnButtonExecuteClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
