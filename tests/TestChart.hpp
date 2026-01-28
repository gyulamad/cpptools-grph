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
    using Chart::timeToX;
    using Chart::valueToY;
    using Chart::innerWidth;
    using Chart::innerHeight;
    using Chart::spacingLeft;
    using Chart::spacingTop;
    using Chart::spacingBottom;
    using Chart::spacingRight;

    // Expose protected methods for testing
    bool testShowBar(time_sec x, float y, unsigned int color) {
        return showBar(x, y, color);
    }

    bool testShowCandleAsLine(const Candle& candle, double candleBodyWidth, unsigned int bullishColor = CHART_COLOR_BULLISH, unsigned int bearishColor = CHART_COLOR_BEARISH) {
        return showCandleAsLine(candle, candleBodyWidth, bullishColor, bearishColor);
    }
};
