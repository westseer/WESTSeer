#ifndef DOWNLOADSDIALOG_H
#define DOWNLOADSDIALOG_H

//(*Headers(DownloadsDialog)
#include <wx/dialog.h>
#include <wx/listctrl.h>
//*)

#define ID_NEW_SCOPE 2001
#define ID_REMOVE_SCOPE 2002
#define ID_REMOVE_ONE_YEAR_FROM_SCOPE 2003

class DownloadsDialog: public wxDialog
{
	public:

		DownloadsDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DownloadsDialog();

		//(*Declarations(DownloadsDialog)
		wxListCtrl* ListCtrlScope;
		//*)

	protected:

		//(*Identifiers(DownloadsDialog)
		static const long ID_LISTCTRL1;
		//*)

	private:

		//(*Handlers(DownloadsDialog)
		void OnKeyDown(wxKeyEvent& event);
		void OnListCtrlScopeItemRClick(wxListEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

		void newScope();
		void removeScope();
		void removeOneYearFromScope();
		void OnPopupClick(wxCommandEvent &event);
};

#endif
