#ifndef TopicIdentification_H
#define TopicIdentification_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <CandidateIdentification.h>
#include <BitermWeight.h>
#include <TermExtraction.h>
#include <map>
#include <string>

class TopicIdentification: public AbstractTask
{
    public:
        TopicIdentification(const std::string path, const std::string kws,
                             TermExtraction *te, BitermWeight *bw, CandidateIdentification *ci);
        virtual ~TopicIdentification();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t,std::pair<std::string,std::string>> *topics);

    protected:
        bool save(int y, std::map<uint64_t,std::pair<std::string,std::string>> &topics);
        bool process(int y);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
        TermExtraction *_te;
        BitermWeight *_bw;
        CandidateIdentification *_ci;
};

#endif // TopicIdentification_H
