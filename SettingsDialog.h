#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//(*Headers(SettingsDialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class SettingsDialog: public wxDialog
{
	public:

		SettingsDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SettingsDialog();

		//(*Declarations(SettingsDialog)
		wxButton* Button7;
		wxButton* ButtonCancel;
		wxButton* ButtonCustomiseDatabase;
		wxButton* ButtonOK;
		wxButton* ButtonResetBiterms;
		wxButton* ButtonResetCitations;
		wxButton* ButtonResetTFIRDF;
		wxButton* ButtonValidateEmail;
		wxNotebook* Notebook1;
		wxPanel* PanelGeneral;
		wxPanel* PanelLLM;
		wxPanel* PanelNNP;
		wxSpinCtrl* SpinCtrlBiterms;
		wxSpinCtrl* SpinCtrlCitations;
		wxSpinCtrlDouble* SpinCtrlTFIRDF;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* StaticText7;
		wxTextCtrl* TextCtrl3;
		wxTextCtrl* TextCtrlDatabase;
		wxTextCtrl* TextCtrlEmail;
		//*)

	protected:

		//(*Identifiers(SettingsDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL2;
		static const long ID_BUTTON6;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRLDOUBLE1;
		static const long ID_BUTTON7;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL3;
		static const long ID_BUTTON8;
		static const long ID_PANEL1;
		static const long ID_PANEL2;
		static const long ID_STATICTEXT7;
		static const long ID_TEXTCTRL3;
		static const long ID_BUTTON9;
		static const long ID_PANEL3;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(SettingsDialog)
		void OnNotebook1PageChanged(wxNotebookEvent& event);
		void OnButtonValidateEmailClick(wxCommandEvent& event);
		void OnButtonCustomiseDatabaseClick(wxCommandEvent& event);
		void OnButtonResetObYearsClick(wxCommandEvent& event);
		void OnButtonResetBitermsClick(wxCommandEvent& event);
		void OnButtonResetTFIRDFClick(wxCommandEvent& event);
		void OnButtonResetCitationsClick(wxCommandEvent& event);
		void OnSpinCtrlObYearsChange(wxSpinEvent& event);
		void OnSpinCtrlBitermsChange(wxSpinEvent& event);
		void OnSpinCtrlTFIRDFChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlCitationsChange(wxSpinEvent& event);
		void OnButtonOKClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

};

#endif
