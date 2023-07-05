#ifndef METRICMODEL_H
#define METRICMODEL_H
#include <AbstractTask.h>
#include <ResearchScope.h>
#include <TimeSeriesExtraction.h>
#include <PredictionModel.h>
#include <vector>

class MetricModel: public AbstractTask
{
    public:
        MetricModel(const std::string path, const std::string kws, TimeSeriesExtraction *tse, PredictionModel *pm);
        virtual ~MetricModel();
        virtual bool finished();
        virtual const char *name();
        virtual int numSteps();
        virtual void doStep(int stepId);
        bool load(int y, std::map<uint64_t, std::vector<double>> *scores);
        static bool load(const std::string keywords, int y, std::map<uint64_t, std::vector<double>> *scores);
        static std::vector<std::string> getScopesWithMetrics(const std::string path);

    protected:
        bool save(int y, const std::map<uint64_t, std::vector<double>> &scores);
        bool process(int y);

    private:
        int _y0;
        int _y1;
        int _y2;
        ResearchScope _scope;
        TimeSeriesExtraction *_tse;
        PredictionModel *_pm;
};

#endif // METRICMODEL_H
