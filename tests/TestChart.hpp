#pragma once

#include "../Chart.hpp"

// Test helper that exposes protected members from Chart
class TestChart : public Chart {
public:
    using Chart::Chart;
    // allow tests to manipulate / inspect protected bounds directly
    void setVisibleRange(time_sec first, time_sec last) { valueFirst = first; valueLast = last; }
    using Chart::valueFirst;
    using Chart::valueLast;
    using Chart::valueLower;
    using Chart::valueUpper;
    using Chart::viewFirst;
    using Chart::viewLast;
    using Chart::timeToX;
    using Chart::valueToY;
    using Chart::innerWidth;
    using Chart::innerHeight;
    using Chart::spacingLeft;
    using Chart::spacingTop;
    using Chart::spacingBottom;
    using Chart::spacingRight;

    // Expose viewInitialized as a getter for testing
    bool isViewInitialized() const { return viewInitialized; }

    // Expose protected methods for testing
    bool testShowBar(time_sec x, float y, unsigned int color) {
        return showBar(x, y, color);
    }

    bool testShowCandleAsLine(const Candle& candle, double candleBodyWidth, unsigned int bullishColor = CHART_COLOR_BULLISH, unsigned int bearishColor = CHART_COLOR_BEARISH) {
        return showCandleAsLine(candle, candleBodyWidth, bullishColor, bearishColor);
    }

    // Expose zoom/scroll methods for testing
    void testZoomAt(double factor, int pixelX) { zoomAt(factor, pixelX); }
    void testScrollBy(double deltaPixels) { scrollBy(deltaPixels); }
    using Chart::zoomAt;
    using Chart::scrollBy;
    using Chart::resetView;
    using Chart::hasDataOutsideView;
    using Chart::getVisibleCandles;
    using Chart::getVisiblePoints;
    using Chart::fitToVisibleCandles;
    using Chart::fitToVisiblePoints;
    using Chart::pixelToTime;
};
