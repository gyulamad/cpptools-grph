#pragma once

#include "../misc/Fl_CanvasBox.hpp"
#include "../trading/Candle.hpp"

#include "Chart.hpp"

class CandleSeries {
public:
    CandleSeries(
        const vector<Candle>& candles,
        const string& interval,
        unsigned int bullishColor = CHART_COLOR_BULLISH,
        unsigned int bearishColor = CHART_COLOR_BEARISH,
        double shoulderSpacing = 0.1
    ):
        candles(candles),
        interval(interval),
        bullishColor(bullishColor),
        bearishColor(bearishColor),
        shoulderSpacing(shoulderSpacing)
    {}

    virtual ~CandleSeries() {}

    const vector<Candle>& getCandlesCRef() const { return candles; }
    const string& getIntervalCRef() const { return interval; }
    unsigned int getBullishColor() const { return bullishColor; }
    unsigned int getBearishColor() const { return bearishColor; }
    double getShoulderSpacing() const { return shoulderSpacing; }

protected:
    const vector<Candle>& candles;
    const string& interval;
    unsigned int bullishColor;
    unsigned int bearishColor;
    double shoulderSpacing;
};

class TimePointSeries {
public:
    TimePointSeries(
        vector<TimePoint>& points,
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
    vector<TimePoint>& points;
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

    void addPointSeries(const TimePointSeries pointSeries) {
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
                candleSeries.getIntervalCRef(), 
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
