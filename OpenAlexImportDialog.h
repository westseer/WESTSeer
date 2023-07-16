#ifndef OPENALEXIMPORTDIALOG_H
#define OPENALEXIMPORTDIALOG_H
#include <ProgressReporter.h>
#include <Publication.h>
#include <vector>
#include <OpenAlex.h>

//(*Headers(OpenAlexImportDialog)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class OpenAlexImportDialog: public wxDialog
{
	public:

		OpenAlexImportDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~OpenAlexImportDialog();
		std::string addScope();

		OpenAlex *_openAlex;

		//(*Declarations(OpenAlexImportDialog)
		wxButton* ButtonCancel;
		wxButton* ButtonGetSamples;
		wxButton* ButtonOK;
		wxChoice* ChoiceKeywordPair;
		wxGauge* GaugeSamplingProgress;
		wxListCtrl* ListCtrlSamples;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* StaticTextSamplingPrompt;
		wxTextCtrl* TextCtrlKeywords;
		wxTextCtrl* TextCtrlSampleAbstract;
		//*)

	protected:

		//(*Identifiers(OpenAlexImportDialog)
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT5;
		static const long ID_GAUGE1;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT7;
		static const long ID_CHOICE1;
		static const long ID_LISTCTRL2;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(OpenAlexImportDialog)
		void OnTextCtrlSampleAbstractText(wxCommandEvent& event);
		void OnButtonGetSamplesClick(wxCommandEvent& event);
		void OnButtonOKClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnChoiceKeywordPairSelect(wxCommandEvent& event);
		void OnListCtrlSamplesItemSelect(wxListEvent& event);
		void OnTextCtrlEmailText(wxCommandEvent& event);
		void OnTextCtrlKeywords2Text(wxCommandEvent& event);
		void OnTextCtrlKeywords1Text(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

		std::vector<std::string> _keywordGroup1;
		std::vector<std::string> _keywordGroup2;
		std::vector<std::string> _keywordPairs;
		std::vector<std::vector<Publication>> _samples;

		void showSamples(int idxKWPair);
		void showAbstract(int idxKWPair, int idxSample);

		class MyProgressReporter: public ProgressReporter
		{
            private:
                OpenAlexImportDialog * _dlg;

            public:
                MyProgressReporter(OpenAlexImportDialog * dlg)
                {
                    _dlg = dlg;
                    _dlg->GaugeSamplingProgress->SetRange(100);
                }
                virtual void report(const char *taskName, int taskId, int numTasks, int taskProgress)
                {
                    int progress = (100 * taskId + taskProgress) / numTasks;
                    if (strcmp(taskName, "Done") == 0 || strcmp(taskName, "Cancelled") == 0)
                    {
                        if (strcmp(taskName, "Done") == 0)
                        {
                            _dlg->_samples = _dlg->_openAlex->samples();
                            const ResearchScope &scope = _dlg->_openAlex->scope();
                            int n = scope.numCombinations();
                            for (int i = 0; i < n; i++)
                            {
                                _dlg->_keywordPairs.push_back(scope.getCombination(i));
                                _dlg->ChoiceKeywordPair->Append(scope.getCombination(i));
                            }
                            if (n > 0)
                            {
                                _dlg->ChoiceKeywordPair->SetSelection(0);
                                _dlg->showSamples(0);
                            }
                        }
                        _dlg->TextCtrlKeywords->Enable();
                        _dlg->ButtonGetSamples->Enable();
                        _dlg->ButtonOK->Enable();
                        _dlg->ChoiceKeywordPair->Enable();
                        _dlg->ListCtrlSamples->Enable();
                    }
                    _dlg->GaugeSamplingProgress->SetValue(progress);
                    _dlg->StaticTextSamplingPrompt->SetLabel(wxString::Format("%d\%", progress));
                }
		} *_progressReporter;
};

#endif
