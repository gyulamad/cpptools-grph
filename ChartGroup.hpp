#pragma once

#include <vector>
#include <algorithm>
#include "Chart.hpp"

using namespace std;

class ChartGroup {
public:
    void addChart(Chart& chart) {
        Chart* ptr = &chart;
        if (find(charts.begin(), charts.end(), ptr) == charts.end()) {
            charts.push_back(ptr);
        }
    }
    
    void removeChart(Chart& chart) {
        Chart* ptr = &chart;
        charts.erase(remove(charts.begin(), charts.end(), ptr), charts.end());
    }
    
    void zoomAt(double factor, int pixelX) {
        // Check if we can zoom out (any chart has data outside view)
        if (factor < 1.0 && !canZoomOut()) return;
        
        for (Chart* chart : charts) {
            chart->zoomAt(factor, pixelX);
        }
    }
    
    void scrollBy(double deltaPixels) {
        for (Chart* chart : charts) {
            chart->scrollBy(deltaPixels);
        }
    }
    
    // Check if any chart has data outside current view
    bool canZoomOut() const {
        for (const Chart* chart : charts) {
            if (chart->hasDataOutsideView()) return true;
        }
        return false;
    }
    
private:
    vector<Chart*> charts;
};
