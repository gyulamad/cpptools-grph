#pragma once

#include "../misc/Fl_CanvasBox.hpp"
#include "../trading/CandleSeries.hpp"
#include "TimePointSeries.hpp"

// Fl_ChartBox will contain a Chart object and handle its drawing
class Fl_ChartBox: public Fl_CanvasBox {
public:
    Fl_ChartBox(
        int X, int Y, int W, int H,
        int spacingTop = CHART_SPACING_TOP,
        int spacingBottom = CHART_SPACING_BOTTOM,
        int spacingLeft = CHART_SPACING_LEFT,
        int spacingRight = CHART_SPACING_RIGHT
    ):
        Fl_CanvasBox(X, Y, W, H),
        chart(
            *static_cast<Canvas*>(this), // Pass itself as Canvas reference
            spacingTop,
            spacingBottom,
            spacingLeft,
            spacingRight
        )
    {}

    virtual ~Fl_ChartBox() {}

    void clearAllSerieses() {
        clearCandlesSerieses();
        clearBarsSerieses();
        clearPointsSerieses();
    }

    void clearCandlesSerieses() {
        for (auto& candlesSeries: candlesSerieses)
            candlesSeries.clear();
    }

    void addCandleSeries(const CandleSeries& candleSeries, size_t pane = 0) {
        while (candlesSerieses.size() < pane + 1) candlesSerieses.push_back({});
        candlesSerieses[pane].push_back(candleSeries);
    }

    void clearBarsSerieses() {
        for (auto& barsSeries: barsSerieses)
            barsSeries.clear();
    }

    void addBarSeries(const TimePointSeries& barSeries, size_t pane = 0) {
        while (barsSerieses.size() < pane + 1) barsSerieses.push_back({});
        barsSerieses[pane].push_back(barSeries);
    }

    void clearPointsSerieses() {
        for (auto& pointsSeries: pointsSerieses)
            pointsSeries.clear();
    }

    void addPointSeries(const TimePointSeries& pointSeries, size_t pane = 0) {
        while (pointsSerieses.size() < pane + 1) pointsSerieses.push_back({});
        pointsSerieses[pane].push_back(pointSeries);
    }

    void draw() override {
        Fl_CanvasBox::draw(); // Call the base class draw method (draws the box itself)
        
        // Fit per pane
        size_t panes = max({ 
            candlesSerieses.size(), 
            barsSerieses.size(),
            pointsSerieses.size(),
        });
        for (size_t pane = 0; pane < panes; pane++) {
            const vector<CandleSeries>& candlesSeries = candlesSerieses.size() > pane ? candlesSerieses[pane] : vector<CandleSeries>();
            const vector<TimePointSeries>& barsSeries = barsSerieses.size() > pane ? barsSerieses[pane] : vector<TimePointSeries>();
            const vector<TimePointSeries>& pointsSeries = pointsSerieses.size() > pane ? pointsSerieses[pane] : vector<TimePointSeries>();

            // Fit the chart to the contents
            chart.resetBounds();
            for (const CandleSeries& candleSeries: candlesSeries)
                chart.fitToCandles(candleSeries.getCandlesCRef());
            for (const TimePointSeries& barSeries: barsSeries)
                chart.fitToPoints(barSeries.getPointsCRef());
            for (const TimePointSeries& pointSeries: pointsSeries)
                chart.fitToPoints(pointSeries.getPointsCRef());

            // Now draw the chart contents        
            for (const CandleSeries& candleSeries: candlesSeries)
                chart.showCandles(
                    candleSeries.getCandlesCRef(), 
                    candleSeries.getInterval(), 
                    candleSeries.getBullishColor(),
                    candleSeries.getBearishColor(),
                    candleSeries.getShoulderSpacing()
                );
            for (const TimePointSeries& barSeries: barsSeries)
                chart.showBars(
                    barSeries.getPointsCRef(), 
                    barSeries.getColor()
                );
            for (const TimePointSeries& pointSeries: pointsSeries)
                chart.showPoints(
                    pointSeries.getPointsCRef(), 
                    pointSeries.getColor()
                );
        }
    }

private:
    Chart chart;

    vector<vector<CandleSeries>> candlesSerieses;
    vector<vector<TimePointSeries>> barsSerieses;
    vector<vector<TimePointSeries>> pointsSerieses;
};
