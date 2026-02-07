#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "Chart.hpp"

using namespace std;

class ChartGroup {
public:
    ChartGroup(bool syncXAxis = true) : syncXAxis(syncXAxis) {}
    
    // Callback to notify when charts need to be redrawn (set by UI_MultiChart)
    function<void()> onSync = nullptr;
    
    void addChart(Chart& chart) {
        Chart* ptr = &chart;
        if (find(charts.begin(), charts.end(), ptr) == charts.end())
            charts.push_back(ptr);
    }
    
    void removeChart(Chart& chart) {
        Chart* ptr = &chart;
        charts.erase(remove(charts.begin(), charts.end(), ptr), charts.end());
    }
    
    void setSyncXAxis(bool sync) { syncXAxis = sync; }
    bool getSyncXAxis() const { return syncXAxis; }
    
    void zoomAt(double factor, int pixelX) {
        if (syncXAxis && !charts.empty()) {
            // Calculate shared data bounds across all charts
            time_sec sharedFirst = charts[0]->getValueFirst();
            time_sec sharedLast = charts[0]->getValueLast();
            for (Chart* chart : charts) {
                sharedFirst = min(sharedFirst, chart->getValueFirst());
                sharedLast = max(sharedLast, chart->getValueLast());
            }
            
            // Apply zoom with shared bounds to all charts
            for (Chart* chart : charts) {
                // Only apply shared bounds if they are valid
                if (sharedFirst < sharedLast) {
                    chart->setValueFirst(sharedFirst);
                    chart->setValueLast(sharedLast);
                }
                chart->zoomAt(factor, pixelX);
            }
            
            // Notify that all charts need to be redrawn
            if (onSync) onSync();
        } else {
            // Independent zoom (original behavior)
            for (Chart* chart : charts)
                chart->zoomAt(factor, pixelX);
        }
    }
    
    void scrollBy(double deltaPixels) {
        if (syncXAxis && !charts.empty()) {
            // Calculate shared data bounds across all charts
            time_sec sharedFirst = charts[0]->getValueFirst();
            time_sec sharedLast = charts[0]->getValueLast();
            for (Chart* chart : charts) {
                sharedFirst = min(sharedFirst, chart->getValueFirst());
                sharedLast = max(sharedLast, chart->getValueLast());
            }
            
            // Apply scroll with shared bounds to all charts
            for (Chart* chart : charts) {
                // Only apply shared bounds if they are valid
                if (sharedFirst < sharedLast) {
                    chart->setValueFirst(sharedFirst);
                    chart->setValueLast(sharedLast);
                }
                chart->scrollBy(deltaPixels);
            }
            
            // Notify that all charts need to be redrawn
            if (onSync) onSync();
        } else {
            // Independent scroll (original behavior)
            for (Chart* chart : charts)
                chart->scrollBy(deltaPixels);
        }
    }
    
    // Synchronize all charts to show the same X-axis range
    void synchronizeXAxis() {
        if (charts.empty()) return;
        
        // Calculate shared data bounds across all charts
        time_sec sharedFirst = charts[0]->getValueFirst();
        time_sec sharedLast = charts[0]->getValueLast();
        for (Chart* chart : charts) {
            sharedFirst = min(sharedFirst, chart->getValueFirst());
            sharedLast = max(sharedLast, chart->getValueLast());
        }
        
        // Apply shared bounds to all charts and reset view
        for (Chart* chart : charts) {
            chart->setValueFirst(sharedFirst);
            chart->setValueLast(sharedLast);
            // Reset view to show full shared range
            chart->setViewFirst(sharedFirst);
            chart->setViewLast(sharedLast);
        }
    }
    
private:
    vector<Chart*> charts;
    bool syncXAxis;
};
