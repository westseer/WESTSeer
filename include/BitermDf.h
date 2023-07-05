#ifndef BITERMDF_H
#define BITERMDF_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <TermTfIrdf.h>
#include <string>
#include <map>

class BitermDf: public AbstractTask
{
    public:
        BitermDf(const std::string path, const std::string kws, TermTfIrdf *tt);
        virtual ~BitermDf();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<std::string, int> *bitermDfs);
        static bool load(const std::string keywords, int y, std::map<std::string, int> *bitermDfs);

    protected:
        bool save(int y, const std::map<std::string, int> &bitermDfs);
        bool process(int y);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
        TermTfIrdf *_tt;
};

#endif // BITERMDF_H
