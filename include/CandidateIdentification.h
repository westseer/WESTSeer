#ifndef CANDIDATEIDENTIFICATION_H
#define CANDIDATEIDENTIFICATION_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <vector>

class CandidateIdentification: public AbstractTask
{
    public:
        CandidateIdentification(const std::string path, const std::string kws);
        virtual ~CandidateIdentification();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::vector<uint64_t> *candidates);
        static bool load(const std::string keywords, int y, std::vector<uint64_t> *candidates);
        static bool removeOneYear(const std::string keywords, int y);

    protected:
        bool save(int y, const std::vector<uint64_t> &candidates);
        bool process(int y);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
};

#endif // CANDIDATEIDENTIFICATION_H
