#ifndef TERMEXTRACTION_H
#define TERMEXTRACTION_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <StopWordMatcher.h>
#include <Matcher.h>

class TermExtraction: public AbstractTask
{
    public:
        TermExtraction(const std::string path, const std::string kws);
        virtual ~TermExtraction();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t, std::map<std::string, std::pair<std::string,int>>> *termFreqs, bool loadTerms = true);
        static bool load(const std::string keywords, int y, std::map<uint64_t, std::map<std::string, std::pair<std::string,int>>> *termFreqs);
        static bool removeOneYear(const std::string keywords, int y);

    protected:
        bool load(int y, std::map<uint64_t, std::vector<std::string>> &texts);
        bool save(int y, const std::map<uint64_t, std::map<std::string, std::pair<std::string, int>>> &termFreqs);

        std::vector<std::vector<std::string>> split(const std::string text);
        bool process(int y);

    private:
        ResearchScope _scope;
        int _y0;
		int _y1;
		int _y2;
		StopWordMatcher _stopWordMatcher;
		Matcher _matcher;
};

#endif // TERMEXTRACTION_H
