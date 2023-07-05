#ifndef PREDICTIONMODEL_H
#define PREDICTIONMODEL_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <TimeSeriesExtraction.h>

class PredictionModel: public AbstractTask
{
    public:
        PredictionModel(const std::string path, const std::string kws, TimeSeriesExtraction *tse);
        virtual ~PredictionModel();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t, std::vector<double>> *prediction);
        static bool load(const std::string keywords, int y, std::map<uint64_t, std::vector<double>> *prediction);

    protected:
        bool save(int y, std::map<uint64_t, std::vector<double>> &prediction);
        bool process(int iStep);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
        TimeSeriesExtraction *_tse;
};

#endif // PREDICTIONMODEL_H
