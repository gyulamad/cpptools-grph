#pragma once

#include <vector>
#include <algorithm>
#include "Chart.hpp"

using namespace std;

class ChartGroup {
public:
    void addChart(Chart& chart) {
        Chart* ptr = &chart;
        if (find(charts.begin(), charts.end(), ptr) == charts.end())
            charts.push_back(ptr);
    }
    
    void removeChart(Chart& chart) {
        Chart* ptr = &chart;
        charts.erase(remove(charts.begin(), charts.end(), ptr), charts.end());
    }
    
    void zoomAt(double factor, int pixelX) {
        for (Chart* chart : charts)
            chart->zoomAt(factor, pixelX);
    }
    
    void scrollBy(double deltaPixels) {
        for (Chart* chart : charts)
            chart->scrollBy(deltaPixels);
    }
    
private:
    vector<Chart*> charts;
};
