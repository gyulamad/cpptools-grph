#pragma once

#include "../misc/Fl_CanvasBox.hpp"
#include "../trading/Candle.hpp"
#include "../misc/array_splice.hpp"

#include "Chart.hpp"
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

    void clearAllSeries() {
        clearCandlesSeries();
        clearPointsSeries();
    }

    void clearCandlesSeries() {
        candlesSeries.clear();
    }

    void addCandleSeries(const CandleSeries& candleSeries) {
        candlesSeries.push_back(candleSeries);
    }

    void clearPointsSeries() {
        pointsSeries.clear();
    }

    void addPointSeries(const TimePointSeries& pointSeries) {
        pointsSeries.push_back(pointSeries);
    }

    void draw() override {
        Fl_CanvasBox::draw(); // Call the base class draw method (draws the box itself)
        
        // Fit the chart to the contents
        chart.resetBounds();
        for (const CandleSeries& candleSeries: candlesSeries)
            chart.fitToCandles(candleSeries.getCandlesCRef());
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
        for (const TimePointSeries& pointSeries: pointsSeries)
            chart.showPoints(
                pointSeries.getPointsCRef(), 
                pointSeries.getColor()
            );
    }

private:
    Chart chart;

    vector<CandleSeries> candlesSeries;
    vector<TimePointSeries> pointsSeries;
};
