#ifndef BITERMWEIGHT_H
#define BITERMWEIGHT_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <TermTfIrdf.h>
#include <BitermDf.h>
#include <string>
#include <map>

class BitermWeight: public AbstractTask
{
    public:
        BitermWeight(const std::string path, const std::string kws, TermTfIrdf *tt, BitermDf *bdf);
        virtual ~BitermWeight();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t, std::map<std::string, double>> *bitermWeights);
        static bool load(const std::string keywords, int y, std::map<uint64_t, std::map<std::string, double>> *bitermWeights);

    protected:
        bool save(int y, std::map<uint64_t, std::map<std::string, double>> &bitermWeights);
        bool process(int y);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
        TermTfIrdf *_tt;
        BitermDf *_bdf;
};

#endif // BITERMWEIGHT_H
