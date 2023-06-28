#include "TimeSeriesRegression.h"
#include <Eigen/Dense>
#include <wxFFileLog.h>
#include <cmath>

TimeSeriesRegression::TimeSeriesRegression(std::vector<double> timeSeries, RegressionType type)
{
    //ctor
    _timeSeries = timeSeries;
    _type = type;
    switch (_type)
    {
    case LINEAR:
        fitLP();
        break;
    case QUADRATIC:
        fitQP();
        break;
    case TP_LINEAR:
        fitTPLP();
        break;
    case TP_QUADRATIC:
        fitTPQP();
        break;
    }
}

TimeSeriesRegression::~TimeSeriesRegression()
{
    //dtor
}

double TimeSeriesRegression::predict(int i)
{
    switch (_type)
    {
    case LINEAR:
        {
            return _params._lp.c0 + _params._lp.c1 * i;
        }
        break;
    case QUADRATIC:
        {
            return _params._qp.c0 + (_params._qp.c1 + _params._qp.c2 * i) * i;
        }
        break;
    case TP_LINEAR:
        {
            double m = 0.5 * ((double)_timeSeries.size() - 1.0);
            return _params._tplp.cM + (i - m) * (i <= m ? _params._tplp.cL  : _params._tplp.cR);
        }
        break;
    case TP_QUADRATIC:
        {
            double m = 0.5 * ((double)_timeSeries.size() - 1.0);
            return _params._tpqp.c0M + (i - m) * (_params._tpqp.c1M
                    + (i - m) * (i <= m ? _params._tpqp.cL : _params._tpqp.cR));
        }
        break;
    default:
        {
            logError("Not implemented.");
            return 0;
        }
    }
}

double TimeSeriesRegression::slope(int i)
{
    switch (_type)
    {
    case LINEAR:
        {
            return _params._lp.c1;
        }
        break;
    case QUADRATIC:
        {
            return _params._qp.c1 + 2.0 * _params._qp.c2 * i;
        }
        break;
    case TP_LINEAR:
        {
            double m = 0.5 * ((double)_timeSeries.size() - 1.0);
            return i <= m ? _params._tplp.cL : _params._tplp.cR;
        }
        break;
    case TP_QUADRATIC:
        {
            double m = 0.5 * ((double)_timeSeries.size() - 1.0);
            return _params._tpqp.c1M + 2.0 * (i - m) * (i <= m ? _params._tpqp.cL : _params._tpqp.cR);
        }
        break;
    default:
        {
            logError("Not implemented.");
            return 0;
        }
    }
}

double TimeSeriesRegression::error()
{
    double err = 0.0;
    for (int i = 0; i < (int)_timeSeries.size(); i++)
    {
        double diff = predict(i) - _timeSeries[i];
        err += diff * diff;
    }
    return sqrt(err);
}

double TimeSeriesRegression::cosError()
{
    double sum11 = 0.0, sum12 = 0.0, sum22 = 0.0;
    for (int i = 0; i < (int)_timeSeries.size(); i++)
    {
        double v1 = predict(i);
        double v2 = _timeSeries[i];
        sum11 += v1 * v1;
        sum12 += v1 * v2;
        sum22 += v2 * v2;
    }
    if (sum11 > 0.0 && sum22 > 0.0)
        return sum12 / sqrt(sum11 * sum22);
    else
        return 0.5;
}

void TimeSeriesRegression::fitLP()
{
    double a0 = 0, a1 = 0, a2 = 0, b0 = 0, b1 = 0;
    for (int i = 0; i < (int)_timeSeries.size(); i++)
    {
        double i2 = i * i;
        a0 += 1.0;
        a1 += i;
        a2 += i2;
        b0 += _timeSeries[i];
        b1 += _timeSeries[i] * i;
    }

    Eigen::Matrix2d A {
        {a0, a1},
        {a1, a2}
    };
    Eigen::Vector2d b(b0, b1);
    Eigen::Vector2d c = A.colPivHouseholderQr().solve(b);
    _params._lp.c0 = c[0];
    _params._lp.c1 = c[1];
}

void TimeSeriesRegression::fitQP()
{
    double a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0, b0 = 0, b1 = 0, b2 = 0;
    for (int i = 0; i < (int)_timeSeries.size(); i++)
    {
        double i2 = i * i;
        double i3 = i2 * i;
        double i4 = i3 * i;
        a0 += 1.0;
        a1 += i;
        a2 += i2;
        a3 += i3;
        a4 += i4;
        b0 += _timeSeries[i];
        b1 += _timeSeries[i] * i;
        b2 += _timeSeries[i] * i2;
    }

    Eigen::Matrix3d A {
        {a0, a1, a2},
        {a1, a2, a3},
        {a2, a3, a4}
    };
    Eigen::Vector3d b(b0, b1, b2);
    Eigen::Vector3d c = A.colPivHouseholderQr().solve(b);
    _params._qp.c0 = c[0];
    _params._qp.c1 = c[1];
    _params._qp.c2 = c[2];
}

void TimeSeriesRegression::fitTPLP()
{
    double m = 0.5 * ((double)_timeSeries.size() - 1.0);
    double a0 = 0, a1L = 0, a1R = 0, a2L = 0, a2R = 0, b0 = 0, b1L = 0, b1R = 0;
    for (int i = 0; i < (int)_timeSeries.size(); i++)
    {
        double di = i - m;
        double di2 = di * di;
        a0 += 1.0;
        b0 += _timeSeries[i];
        if (di <= 0.0)
        {
            a1L += di;
            a2L += di2;
            b1L += _timeSeries[i] * di;
        }
        else
        {
            a1R += di;
            a2R += di2;
            b1R += _timeSeries[i] * di;
        }
    }

    Eigen::Matrix3d A {
        {a0, a1L, a1R},
        {a1L, a2L, 0.0},
        {a1R, 0.0, a2R}
    };
    Eigen::Vector3d b(b0, b1L, b1R);
    Eigen::Vector3d c = A.colPivHouseholderQr().solve(b);
    _params._tplp.cM = c[0];
    _params._tplp.cL = c[1];
    _params._tplp.cR = c[2];
}

void TimeSeriesRegression::fitTPQP()
{
    double m = 0.5 * ((double)_timeSeries.size() - 1.0);
    double a0 = 0, a1 = 0, a2 = 0, a2L = 0, a2R = 0, a3L = 0, a3R = 0, a4L = 0, a4R = 0;
    double b0 = 0, b1 = 0, b2L = 0, b2R = 0;
    for (int i = 0; i < (int)_timeSeries.size(); i++)
    {
        double di = i - m;
        double di2 = di * di;
        double di3 = di2 * di;
        double di4 = di3 * di;
        a0 += 1.0;
        a1 += di;
        a2 += di2;
        b0 += _timeSeries[i];
        b1 += _timeSeries[i] * di;
        if (di <= 0.0)
        {
            a2L += di2;
            a3L += di3;
            a4L += di4;
            b2L += _timeSeries[i] * di2;
        }
        else
        {
            a2R += di2;
            a3R += di3;
            a4R += di4;
            b2R += _timeSeries[i] * di2;
        }
    }

    Eigen::Matrix4d A {
        {a0, a1, a2L, a2R},
        {a1, a2, a3L, a3R},
        {a2L, a3L, a4L, 0},
        {a2R, a3R, 0, a4R}
    };
    Eigen::Vector4d b(b0, b1, b2L, b2R);
    Eigen::Vector4d c = A.colPivHouseholderQr().solve(b);
    _params._tpqp.c0M = c[0];
    _params._tpqp.c1M = c[1];
    _params._tpqp.cL = c[2];
    _params._tpqp.cR = c[3];
}
