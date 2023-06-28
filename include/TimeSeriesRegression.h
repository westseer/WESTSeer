#ifndef TIMESERIESREGRESSION_H
#define TIMESERIESREGRESSION_H
#include <vector>

class TimeSeriesRegression
{
    public:
        enum RegressionType
        {
            LINEAR,
            QUADRATIC,
            TP_LINEAR,
            TP_QUADRATIC
        };
        TimeSeriesRegression(std::vector<double> timeSeries, RegressionType type);
        virtual ~TimeSeriesRegression();
        double predict(int i);
        double slope(int i);
        double error();
        double cosError();

    protected:
        void fitLP();
        void fitQP();
        void fitTPLP();
        void fitTPQP();

    private:
        std::vector<double> _timeSeries;
        RegressionType _type;
        struct LinearParams
        {
            double c0, c1;
        };
        struct QuadraticParams
        {
            double c0, c1, c2;
        };
        struct TPLinearParams
        {
            double cM, cL, cR;
        };
        struct TPQuadraticParams
        {
            double c0M, c1M, cL, cR;
        };
        union
        {
            LinearParams _lp;
            QuadraticParams _qp;
            TPLinearParams _tplp;
            TPQuadraticParams _tpqp;
        } _params;
};

#endif // TIMESERIESREGRESSION_H
