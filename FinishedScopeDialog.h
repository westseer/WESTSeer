#ifndef FINISHEDSCOPEDIALOG_H
#define FINISHEDSCOPEDIALOG_H

//(*Headers(FinishedScopeDialog)
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <OpenAlex.h>
#include <TermExtraction.h>
#include <TermTfIrdf.h>
#include <BitermDf.h>
#include <BitermWeight.h>
#include <CandidateIdentification.h>
#include <TopicIdentification.h>
#include <TimeSeriesExtraction.h>
#include <PredictionModel.h>
#include <MetricModel.h>


class FinishedScopeDialog: public wxDialog
{
	public:

		FinishedScopeDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~FinishedScopeDialog();

		//(*Declarations(FinishedScopeDialog)
		wxChoice* ChoiceScope;
		wxListCtrl* ListCtrlCitations;
		wxListCtrl* ListCtrlPublications;
		wxListCtrl* ListCtrlReferences;
		wxNotebook* NotebookInfo;
		wxRadioBox* RadioBoxMode;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxTextCtrl* TextCtrlAbstract;
		wxTextCtrl* TextCtrlPrediction;
		wxTextCtrl* TextCtrlTopic;
		wxTextCtrl* TextCtrlVerification;
		//*)

	protected:

		//(*Identifiers(FinishedScopeDialog)
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT2;
		static const long ID_RADIOBOX1;
		static const long ID_LISTCTRL1;
		static const long ID_LISTCTRL2;
		static const long ID_LISTCTRL3;
		static const long ID_TEXTCTRL1;
		static const long ID_TEXTCTRL2;
		static const long ID_TEXTCTRL3;
		static const long ID_TEXTCTRL4;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(FinishedScopeDialog)
		void OnRadioBoxModeSelect(wxCommandEvent& event);
		void OnChoiceScopeSelect(wxCommandEvent& event);
		void OnListCtrlPublicationsItemSelect(wxListEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

		bool _exploreMode;
        std::vector<uint64_t> _ids;
        std::vector<int> _vRanks;
        std::map<uint64_t,TSM> _timeSeries;
        std::map<uint64_t, std::pair<std::string,std::string>> _topics;
        std::map<uint64_t, std::vector<double>> _scores;
        std::map<uint64_t, std::vector<Publication>> _citations;
        time_t _timeStart;
        std::thread *_displayThread;

        void showCandidates();
        void showCandidate(uint64_t id);
};

#endif
