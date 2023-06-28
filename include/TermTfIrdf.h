#ifndef TERMTFIRDF_H
#define TERMTFIRDF_H
#include <AbstractTask.h>
#include <TermExtraction.h>
#include <string>
#include <map>

class TermTfIrdf: public AbstractTask
{
    public:
        TermTfIrdf(const std::string path, const std::string kws, TermExtraction *te);
        virtual ~TermTfIrdf();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t, std::map<std::string, double>> *tfirdfs, bool loadDfs = true);

    protected:
        bool save(int y, const std::map<uint64_t, std::map<std::string, double>> &tfirdfs);
        bool process(int y);

    private:
        ResearchScope _scope;
        TermExtraction *_te;
        int _y0;
		int _y1;
		int _y2;
		std::map<std::string, int> _dfs;
		int _numWorks;
};

#endif // TERMTFIRDF_H
