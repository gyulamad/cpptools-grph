#pragma once

#include "../Chart.hpp"

// Test helper that overrides showLine to return false for testing failure scenarios
class MockShowLineChart : public Chart {
public:
    using Chart::Chart;

    // Override showLine to return false
    bool showLine(time_sec x1, float y1, time_sec x2, float y2, unsigned int color) override {
        return false;
    }

    // Public wrapper for protected showCandleAsLine method
    bool showCandleAsLinePublic(const Candle& candle, double candleBodyWidth, unsigned int upColor, unsigned int downColor) {
        return showCandleAsLine(candle, candleBodyWidth, upColor, downColor);
    }
};
