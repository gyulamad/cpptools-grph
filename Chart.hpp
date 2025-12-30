#pragma once

#include "../misc/EGA_COLORS.hpp"
#include "../misc/Canvas.hpp"
#include "TimePoint.hpp"
#include <cmath>
#include "../trading/Candle.hpp"

using namespace std;

const unsigned int CHART_COLOR_BULLISH = EGA_GREEN;
const unsigned int CHART_COLOR_BEARISH = EGA_RED;
const unsigned int CHART_COLOR_PLOTTER = EGA_LIGHT_GRAY;

const int CHART_SPACING_TOP = 30;
const int CHART_SPACING_BOTTOM = 30;
const int CHART_SPACING_LEFT = 100;
const int CHART_SPACING_RIGHT = 100;

class Chart {
public:
    Chart(
        Canvas& canvas,
        int spacingTop = CHART_SPACING_TOP,
        int spacingBottom = CHART_SPACING_BOTTOM,
        int spacingLeft = CHART_SPACING_LEFT,
        int spacingRight = CHART_SPACING_RIGHT
    ): 
        canvas(canvas),
        spacingTop(spacingTop),
        spacingBottom(spacingBottom),
        spacingLeft(spacingLeft),
        spacingRight(spacingRight)
    {}

    virtual ~Chart() {}

    void resetBounds() {
        valueFirst = numeric_limits<time_sec>::max();
        valueLast = numeric_limits<time_sec>::min();
        valueUpper = -numeric_limits<float>::infinity();
        valueLower = numeric_limits<float>::infinity();
    }

    void fitToCandles(const vector<Candle>& candles) {
        for (const Candle& candle: candles) {
            const time_sec candleTime = candle.getTime();
            const float candleLow = candle.getLow();
            if (isnan(candleLow)) continue;
            const float candleHigh = candle.getHigh();
            if (isnan(candleHigh)) continue;
            valueFirst = candleTime < valueFirst ? candleTime : valueFirst;
            valueLast = candleTime > valueLast ? candleTime : valueLast;
            valueLower = candleLow < valueLower ? candleLow : valueLower;
            valueUpper = candleHigh > valueUpper ? candleHigh : valueUpper;
        }
    }
    
    void fitToPoints(const vector<TimePoint>& points) {
        for (const TimePoint& point: points) {
            const time_sec pointTime = point.getTime();
            const float pointValue = point.getValue();
            if (isnan(pointValue)) continue;
            valueFirst = valueFirst < pointTime ? valueFirst : pointTime;
            valueLast = valueLast > pointTime ? valueLast : pointTime;
            valueLower = valueLower < pointValue ? valueLower : pointValue;
            valueUpper = valueUpper > pointValue ? valueUpper : pointValue;
        }
    }

    void showCandles(
        const vector<Candle>& candles,
        time_sec interval,
        unsigned int bullishColor = CHART_COLOR_BULLISH, 
        unsigned int bearishColor = CHART_COLOR_BEARISH,
        double shoulderSpacing = 0.1
    ) {
        //  Calculate the candle body with in pixels (double) from interval
        time_sec totalSeconds = valueLast - valueFirst; // Calculate the total time span of the chart        
        int canvasWidth = innerWidth(); // Use inner width instead of full canvas width
        double candleBodyWidth = (double)canvasWidth * interval / totalSeconds; // Calculate the width of one interval in pixels
        
        // Select the right level of details (LOD)
        if (candleBodyWidth > 5) { // Show each candles...
            for (const Candle& candle: candles) 
                if (!showCandle(
                    candle, candleBodyWidth, 
                    bullishColor, bearishColor, 
                    shoulderSpacing
                )) continue;
            return;
        }

        if (candleBodyWidth >= 1) { // Show only a representing line
            for (const Candle& candle: candles) 
                if (!showCandleAsLine(candle, candleBodyWidth, bullishColor, bearishColor)) continue;
            return;
        }

        Candle prevCandle = candles[0];
        int step = 1 / candleBodyWidth;
        for (size_t n = step; n < candles.size(); n += step) {
            const Candle& thisCandle = candles[n];
            
            float open = prevCandle.getOpen();
            float close = thisCandle.getClose();
            float low = prevCandle.getLow();
            if (thisCandle.getLow() < low) low = thisCandle.getLow();
            float high = prevCandle.getHigh();
            if (thisCandle.getHigh() > high) high = thisCandle.getHigh();                        
            Candle candle(thisCandle.getTime(), open, high, low, close, thisCandle.getVolume());
            if (!showCandleAsLine(candle, candleBodyWidth, bullishColor, bearishColor)) continue;

            prevCandle = thisCandle;
        }

    }

    void showBars(
        const vector<TimePoint>& points,
        unsigned int color = CHART_COLOR_PLOTTER,
        double spacing = 0.1 // TODO
    ) {
        // If we don't have a valid time range or drawable width, bail out
        if (points.size() < 2) return;
        if (valueLast < valueFirst) return;
        int widthPx = innerWidth();
        if (widthPx <= 0) return;

        // How many seconds correspond to one pixel (computed once)
        double secondsPerPixel = static_cast<double>(valueLast - valueFirst) / static_cast<double>(widthPx);

        time_sec t1, t2;
        float v2;
        bool first = true;

        const int lod = 1; // number of pixels to skip
        double lodSeconds = lod * secondsPerPixel; // computed once, in seconds

        for (const TimePoint& point: points) {
            if (first) {
                t1 = point.getTime();
                first = false;
                continue;
            }
            t2 = point.getTime();
            
            // compare time difference in seconds
            time_sec dt = t2 > t1 ? t2 - t1 : t1 - t2;
            if (dt < lodSeconds) continue;

            v2 = point.getValue();
            if (!showBar(t2, v2, color)) continue;
            t1 = t2;
        }
    }


    void showPoints(
        const vector<TimePoint>& points,
        unsigned int color = CHART_COLOR_PLOTTER
    ) {
        // If we don't have a valid time range or drawable width, bail out
        if (points.size() < 2) return;
        if (valueLast < valueFirst) return;
        int widthPx = innerWidth();
        if (widthPx <= 0) return;

        // How many seconds correspond to one pixel (computed once)
        double secondsPerPixel = static_cast<double>(valueLast - valueFirst) / static_cast<double>(widthPx);

        time_sec t1, t2;
        float v1, v2;
        bool first = true;

        const int lod = 1; // number of pixels to skip
        double lodSeconds = lod * secondsPerPixel; // computed once, in seconds

        for (const TimePoint& point: points) {
            if (first) {
                t1 = point.getTime();
                v1 = point.getValue();
                first = false;
                continue;
            }
            t2 = point.getTime();
            
            // compare time difference in seconds
            time_sec dt = t2 > t1 ? t2 - t1 : t1 - t2;
            if (dt < lodSeconds) continue;

            v2 = point.getValue();
            if (!showLine(t1, v1, t2, v2, color)) continue;
            t1 = t2;
            v1 = v2;
        }
    }

protected:
    // Helper methods to get inner drawing area dimensions
    int innerWidth() const {
        return canvas.width() - spacingLeft - spacingRight;
    }
    
    int innerHeight() const {
        return canvas.height() - spacingTop - spacingBottom;
    }
    
    int innerX(int x) const {
        return x + spacingLeft;
    }
    
    int innerY(int y) const {
        return y + spacingTop;
    }

    [[nodiscard]] 
    bool showCandle(
        const Candle& candle, 
        double candleBodyWidth, 
        unsigned int bullishColor = CHART_COLOR_BULLISH, 
        unsigned int bearishColor = CHART_COLOR_BEARISH,
        double shoulderSpacing = 0.1
    ) {
        time_sec time = candle.getTime();
        float open = candle.getOpen();
        if (isnan(open)) return false;
        float high = candle.getHigh();
        if (isnan(high)) return false;
        float low = candle.getLow();
        if (isnan(low)) return false;
        float close = candle.getClose();
        if (isnan(close)) return false;

        unsigned int color = (close > open) ? bullishColor : bearishColor;

        // Wick
        if (!showLine(time, high, time, low, color)) return false;

        // Convert time to pixel coordinates
        int centerX = timeToX(time);
        double bodyWidthPx = candleBodyWidth * (1.0 - 2.0 * shoulderSpacing);

        int left = (int)(centerX - bodyWidthPx / 2.0);
        int top = min(valueToY(open), valueToY(close));
        int bottom = max(valueToY(open), valueToY(close));

        // Convert to width & height
        int widthPx  = (int)bodyWidthPx;
        int heightPx = bottom - top;

        canvas.rectf(left, top, widthPx, heightPx, color);
        return true;
    }

    [[nodiscard]] 
    bool showCandleAsLine(
        const Candle& candle,
        double candleBodyWidth, 
        unsigned int bullishColor = CHART_COLOR_BULLISH, 
        unsigned int bearishColor = CHART_COLOR_BEARISH
    ) {
        time_sec candleTime = candle.getTime();
        float open = candle.getOpen();
        if (isnan(open)) return false;
        float high = candle.getHigh();
        if (isnan(high)) return false;
        float low = candle.getLow();
        if (isnan(low)) return false;
        float close = candle.getClose();
        if (isnan(close)) return false;
        if (!showLine(
            candleTime, candle.getLow(), 
            candleTime + candleBodyWidth, candle.getHigh(), 
            candle.getOpen() < candle.getClose() ? bullishColor : bearishColor
        )) return false;
        return true;
    }

    [[nodiscard]]
    bool showBar(
        time_sec x, float y,
        unsigned int color
    ) {
        if (isnan(y)) return false;
        canvas.line(
            timeToX(x), valueToY(y), 
            timeToX(x), valueToY(0), 
            color
        );
        return true;
    }

    [[nodiscard]]
    bool showLine(
        time_sec x1, float y1,
        time_sec x2, float y2,
        unsigned int color
    ) {
        if (isnan(y1) || isnan(y2)) return false;
        canvas.line(
            timeToX(x1), valueToY(y1), 
            timeToX(x2), valueToY(y2), 
            color
        );
        return true;
    }

    // Project time_sec value to x coordinate on canvas (with padding)
    int timeToX(time_sec time) const {
        if (valueLast == valueFirst) return innerWidth() / 2; // Avoid division by zero
        
        // Linear interpolation: map [valueFirst, valueLast] to [spacingLeft, canvas.width()-spacingRight]
        double ratio = static_cast<double>(time - valueFirst) / (valueLast - valueFirst);
        int x = static_cast<int>(ratio * innerWidth());
        return innerX(x); // Add left spacing
    }
    
    // Project float value to y coordinate on canvas (with padding)
    int valueToY(float value) const {
        if (valueUpper == valueLower) return innerHeight() / 2; // Avoid division by zero
        
        // Linear interpolation: map [valueLower, valueUpper] to [canvas.height()-spacingBottom, spacingTop]
        // Note: Y axis is inverted (0 is at top)
        double ratio = static_cast<double>(value - valueLower) / (valueUpper - valueLower);
        int y = innerHeight() - static_cast<int>(ratio * innerHeight());
        return innerY(y); // Add top spacing
    }

    Canvas& canvas;
    int spacingTop;
    int spacingBottom;
    int spacingLeft;
    int spacingRight;
    time_sec valueFirst;
    time_sec valueLast;
    float valueUpper;
    float valueLower;
};
