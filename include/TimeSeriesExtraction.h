#ifndef TimeSeriesEXTRACTION_H
#define TimeSeriesEXTRACTION_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <BitermWeight.h>
#include <CandidateIdentification.h>
#include <TopicIdentification.h>
#include <map>

typedef std::pair<std::pair<std::vector<int>,std::vector<int>>,std::pair<std::vector<int>,std::vector<int>>> TSM;
class TimeSeriesExtraction: public AbstractTask
{
    public:
        TimeSeriesExtraction(const std::string path, const std::string kws,
                               BitermWeight *bw, CandidateIdentification *ci, TopicIdentification *ti);
        virtual ~TimeSeriesExtraction();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t, TSM> *timeSeries);
        static bool load(const std::string keywords, int y, std::map<uint64_t, TSM> *timeSeries);
        static bool removeOneYear(const std::string keywords, int y);

    protected:
        bool save(int y, const std::map<uint64_t, TSM> &timeSeries);
        bool process(int y);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
        BitermWeight *_bw;
        CandidateIdentification *_ci;
        TopicIdentification *_ti;
};

#endif // TimeSeriesEXTRACTION_H
