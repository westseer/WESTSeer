#ifndef __OPENALEX_H__
#define __OPENALEX_H__
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdint.h>
#include <AbstractTask.h>
#include <Publication.h>
#include <ResearchScope.h>
#include <ProgressReporter.h>

class OpenAlex: public AbstractTask {
	private:
		std::string _email;
		ResearchScope _scope;
		int _y0;
		int _y1;
		int _y2;
		std::vector<std::pair<int,std::vector<std::string>>> _urls;
		bool _samplesOnly;
		std::vector<std::vector<Publication>> _samples;

	public:
		OpenAlex(const std::string email, const std::string path,
            const std::string kws1, const std::string kws2);
        OpenAlex(const std::string email, const std::string path,
            const std::string kws);
		~OpenAlex();
		virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);

		inline const ResearchScope &scope()
		{
		    return _scope;
		}
		inline const std::vector<Publication>& samples(int idxComb)
		{
		    return _samples[idxComb];
		}
		inline const std::vector<std::vector<Publication>>& samples()
		{
		    return _samples;
		}
		inline void setSamplesOnly(bool value)
		{
		    _samplesOnly = value;
		}

	protected:
	    void init();

};
#endif
