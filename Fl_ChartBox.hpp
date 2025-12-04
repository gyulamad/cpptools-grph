#pragma once

#include "../misc/Fl_CanvasBox.hpp"
#include "../trading/Candle.hpp"

#include "Chart.hpp"

struct CandleSeries {
    vector<Candle> candles;
    string interval;
    unsigned int bullishColor = CHART_COLOR_BULLISH;
    unsigned int bearishColor = CHART_COLOR_BEARISH;
    double shoulderSpacing = 0.1;
};

struct TimePointSeries {
    vector<TimePoint> points;
    unsigned int color = CHART_COLOR_PLOTTER;
};

// Fl_ChartBox will contain a Chart object and handle its drawing
class Fl_ChartBox : public Fl_CanvasBox {
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

    void clearAll() {
        clearCandles();
        clearPoints();
    }

    void clearCandles() {
        candles.clear();
    }

    void addCandles(
        const vector<Candle>& candles, 
        const string& interval,
        unsigned int bullishColor = CHART_COLOR_BULLISH,
        unsigned int bearishColor = CHART_COLOR_BEARISH,
        double shoulderSpacing = 0.1
    ) {
        this->candles.push_back({ 
            candles, 
            interval, 
            bullishColor, 
            bearishColor, 
            shoulderSpacing
        });
    }

    void clearPoints() {
        points.clear();
    }

    void addPoints(
        const vector<TimePoint>& points, 
        unsigned int color = CHART_COLOR_PLOTTER
    ) {
        this->points.push_back({ points, color });
    }

    void draw() override {
        Fl_CanvasBox::draw(); // Call the base class draw method (draws the box itself)
        
        // Fit the chart to the contents
        chart.resetBounds();
        for (const CandleSeries& content: candles)
            chart.fitToCandles(content.candles);
        for (const TimePointSeries& content: points)
            chart.fitToPoints(content.points);

        // Now draw the chart contents        
        for (const CandleSeries& content: candles)
            chart.showCandles(content.candles, content.interval, content.bullishColor, content.bearishColor, content.shoulderSpacing);
        for (const TimePointSeries& content: points)
            chart.showPoints(content.points, content.color);
    }

private:
    Chart chart;

    vector<CandleSeries> candles;
    vector<TimePointSeries> points;
};
