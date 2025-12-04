#pragma once

#include "../misc/Canvas.hpp"
#include "../misc/EGA_COLORS.hpp"

#include "../trading/Candle.hpp"
#include "../trading/intervalToSecond.hpp"


#include <FL/Fl.H>          // Main FLTK header


const unsigned int CHART_GREEN = EGA_GREEN;
const unsigned int CHART_RED = EGA_RED;

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

    void showCandles(
        const vector<Candle>& candles,
        const string& interval,
        unsigned int bullishColor = CHART_GREEN, 
        unsigned int bearishColor = CHART_RED,
        double shoulderSpacing = 0.1
    ) {
        // Fit the chart bounds to strech candles...
        valueFirst = numeric_limits<time_sec>::max();
        valueLast = numeric_limits<time_sec>::min();
        valueUpper = -numeric_limits<float>::infinity();
        valueLower = numeric_limits<float>::infinity();
        for (const Candle& candle: candles) {
            time_sec candleTime = candle.getTime();
            float candleLow = candle.getLow();
            float candleHigh = candle.getHigh();
            valueFirst = candleTime < valueFirst ? candleTime : valueFirst;
            valueLast = candleTime > valueLast ? candleTime : valueLast;
            valueLower = candleLow < valueLower ? candleLow : valueLower;
            valueUpper = candleHigh > valueUpper ? candleHigh : valueUpper;
        }

        //  Calculate the candle body with in pixels (double) from interval
        time_sec intervalSeconds = intervalToSecond(interval); // Convert interval string to seconds
        time_sec totalSeconds = valueLast - valueFirst; // Calculate the total time span of the chart        
        int canvasWidth = innerWidth(); // Use inner width instead of full canvas width
        double candleBodyWidth = (double)canvasWidth * intervalSeconds / totalSeconds; // Calculate the width of one interval in pixels
        
        // Select the right level of details (LOD)
        if (candleBodyWidth > 5) { // Show each candles...
            for (const Candle& candle: candles) 
                showCandle(
                    candle, candleBodyWidth, 
                    bullishColor, bearishColor, 
                    shoulderSpacing
                );
            return;
        }

        if (candleBodyWidth >= 1) { // Show only a representing line
            for (const Candle& candle: candles) 
                showCandleAsLine(candle, candleBodyWidth, bullishColor, bearishColor);
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
            showCandleAsLine(candle, candleBodyWidth, bullishColor, bearishColor);

            prevCandle = thisCandle;
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

    void showCandle(
        const Candle& candle, 
        double candleBodyWidth, 
        unsigned int bullishColor = CHART_GREEN, 
        unsigned int bearishColor = CHART_RED,
        double shoulderSpacing = 0.1
    ) {
        time_sec time = candle.getTime();
        float open  = candle.getOpen();
        float high  = candle.getHigh();
        float low   = candle.getLow();
        float close = candle.getClose();

        unsigned int color = (close > open) ? bullishColor : bearishColor;

        // Wick
        showLine(time, high, time, low, color);

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
    }

    void showCandleAsLine(
        const Candle& candle,
        double candleBodyWidth, 
        unsigned int bullishColor = CHART_GREEN, 
        unsigned int bearishColor = CHART_RED
    ) {
        time_sec candleTime = candle.getTime();
        showLine(
            candleTime, candle.getLow(), 
            candleTime + candleBodyWidth, candle.getHigh(), 
            candle.getOpen() < candle.getClose() ? bullishColor : bearishColor
        );
    }

    void showLine(
        time_sec x1, float y1,
        time_sec x2, float y2,
        unsigned int color
    ) {
        canvas.line(
            timeToX(x1), valueToY(y1), 
            timeToX(x2), valueToY(y2), 
            color
        );
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
