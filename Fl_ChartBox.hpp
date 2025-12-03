#pragma once

#include "../misc/Fl_CanvasBox.hpp"
#include "../trading/Candle.hpp"

#include "Chart.hpp"

// Fl_ChartBox will contain a Chart object and handle its drawing
class Fl_ChartBox : public Fl_CanvasBox {
public:
    Fl_ChartBox(
        int X, int Y, int W, int H,
        const vector<Candle>& candles, 
        const string& interval,
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
        ),
        candles(candles),
        interval(interval)
    {}

    void draw() override {
        Fl_CanvasBox::draw(); // Call the base class draw method (draws the box itself)
        // Now draw the chart content
        chart.showCandles(candles, interval);
    }

private:
    Chart chart;
    vector<Candle> candles;
    string interval;
};
