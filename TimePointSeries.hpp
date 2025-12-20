#pragma once

#include "TimePoint.hpp"
#include "Chart.hpp"

using namespace std;

class TimePointSeries {
public:
    TimePointSeries(
        vector<TimePoint> points,
        unsigned int color = CHART_COLOR_PLOTTER
    ):
        points(points),
        color(color)
    {}

    virtual ~TimePointSeries() {}

    vector<TimePoint>& getPointsRef() { return points; }
    const vector<TimePoint>& getPointsCRef() const { return points; }
    unsigned int getColor() const { return color; }

protected:
    vector<TimePoint> points;
    unsigned int color = CHART_COLOR_PLOTTER;
};
