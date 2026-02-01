#pragma once

#include "../misc/EGA_COLORS.hpp"
#include "../misc/Canvas.hpp"
#include "../misc/Logger.hpp"
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
        int spacingRight = CHART_SPACING_RIGHT,
        double zoomInFactor = 1.25,
        double zoomOutFactor = 0.8
    ): 
        canvas(canvas),
        spacingTop(spacingTop),
        spacingBottom(spacingBottom),
        spacingLeft(spacingLeft),
        spacingRight(spacingRight),
        zoomInFactor(zoomInFactor),
        zoomOutFactor(zoomOutFactor)
    {
        resetBounds();
    }

    virtual ~Chart() {}
    
    // Public getters for canvas dimensions
    int getCanvasWidth() const { return canvas.width(); }
    int getCanvasHeight() const { return canvas.height(); }

    void resetBounds() {
        valueFirst = numeric_limits<time_sec>::max();
        valueLast = numeric_limits<time_sec>::min();
        valueUpper = -numeric_limits<float>::infinity();
        valueLower = numeric_limits<float>::infinity();
        // NOTE: Do NOT reset viewFirst/viewLast here - view persists across draw calls
        // The view is only initialized via resetView() or modified via zoomAt()/scrollBy()
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
    
    // Reset view to full data range (only if not already initialized)
    void resetView() {
        if (viewInitialized) return;
        viewFirst = valueFirst;
        viewLast = valueLast;
        viewInitialized = true;
    }
    
    // Getters for zoom factors
    double getZoomInFactor() const { return zoomInFactor; }
    double getZoomOutFactor() const { return zoomOutFactor; }
    
    // Getters for view bounds
    time_sec getViewFirst() const { return viewFirst; }
    time_sec getViewLast() const { return viewLast; }
    bool isViewInitialized() const { return viewInitialized; }
    
    // Getters for data bounds (for use by Fl_ChartBox)
    time_sec getValueFirst() const { return valueFirst; }
    time_sec getValueLast() const { return valueLast; }
    void setValueFirst(time_sec v) { valueFirst = v; }
    void setValueLast(time_sec v) { valueLast = v; }

    // Check if any data is outside visible view
    bool hasDataOutsideView() const {
        return valueFirst < viewFirst || valueLast > viewLast;
    }
    
    // Get visible subset of candles
    vector<Candle> getVisibleCandles(const vector<Candle>& candles) const {
        // If view not initialized, return all candles (view not yet set)
        if (!viewInitialized) {
            DBG("Chart::getVisibleCandles() - view not initialized, returning all candles");
            return candles;
        }
        
        DBG("Chart::getVisibleCandles() - viewFirst=" + to_string(viewFirst) + ", viewLast=" + to_string(viewLast) + ", dataFirst=" + to_string(valueFirst) + ", dataLast=" + to_string(valueLast));
        vector<Candle> visible;
        for (const Candle& candle : candles) {
            time_sec t = candle.getTime();
            if (t >= viewFirst && t <= viewLast)
                visible.push_back(candle);
        }
        DBG("Chart::getVisibleCandles() - filtered " + to_string(visible.size()) + " of " + to_string(candles.size()) + " candles");
        return visible;
    }

    // Get visible subset of points
    vector<TimePoint> getVisiblePoints(const vector<TimePoint>& points) const {
        // If view not initialized, return all points (view not yet set)
        if (!viewInitialized) {
            DBG("Chart::getVisiblePoints() - view not initialized, returning all points");
            return points;
        }
        
        DBG("Chart::getVisiblePoints() - viewFirst=" + to_string(viewFirst) + ", viewLast=" + to_string(viewLast));
        vector<TimePoint> visible;
        for (const TimePoint& point : points) {
            time_sec t = point.getTime();
            if (t >= viewFirst && t <= viewLast)
                visible.push_back(point);
        }
        DBG("Chart::getVisiblePoints() - filtered " + to_string(visible.size()) + " of " + to_string(points.size()) + " points");
        return visible;
    }

    // Fit Y-axis to visible candles only (updates value bounds to visible subset)
    void fitToVisibleCandles(const vector<Candle>& candles) {
        // First get visible subset, then update bounds
        vector<Candle> visible = getVisibleCandles(candles);
        
        // If no visible candles, preserve original bounds
        if (visible.empty()) {
            DBG("Chart::fitToVisibleCandles() - no visible candles, preserving original bounds");
            return;
        }
        
        DBG("Chart::fitToVisibleCandles() - fitting Y-axis to " + to_string(visible.size()) + " visible candles");
        
        // Only update Y-axis bounds (valueLower/valueUpper), NOT time bounds (valueFirst/valueLast)
        // valueFirst/valueLast should preserve the full data range for zoom calculations
        float newValueLower = numeric_limits<float>::infinity();
        float newValueUpper = -numeric_limits<float>::infinity();
        
        for (const Candle& candle : visible) {
            const float candleLow = candle.getLow();
            if (isnan(candleLow)) continue;
            const float candleHigh = candle.getHigh();
            if (isnan(candleHigh)) continue;
            
            // Update value bounds only
            if (candleLow < newValueLower) newValueLower = candleLow;
            if (candleHigh > newValueUpper) newValueUpper = candleHigh;
        }
        
        // Only update if we found valid values
        if (newValueLower != numeric_limits<float>::infinity()) {
            valueLower = newValueLower;
            valueUpper = newValueUpper;
            DBG("Chart::fitToVisibleCandles() - new Y bounds: valueLower=" + to_string(valueLower) + ", valueUpper=" + to_string(valueUpper));
        } else {
            DBG("Chart::fitToVisibleCandles() - no valid candle values found");
        }
    }

    // Fit Y-axis to visible points only (updates value bounds to visible subset)
    void fitToVisiblePoints(const vector<TimePoint>& points) {
        // First get visible subset, then update bounds
        vector<TimePoint> visible = getVisiblePoints(points);
        
        // If no visible points, preserve original bounds
        if (visible.empty()) {
            DBG("Chart::fitToVisiblePoints() - no visible points, preserving original bounds");
            return;
        }
        
        DBG("Chart::fitToVisiblePoints() - fitting Y-axis to " + to_string(visible.size()) + " visible points");
        
        // Only update Y-axis bounds (valueLower/valueUpper), NOT time bounds (valueFirst/valueLast)
        // valueFirst/valueLast should preserve the full data range for zoom calculations
        float newValueLower = numeric_limits<float>::infinity();
        float newValueUpper = -numeric_limits<float>::infinity();
        
        for (const TimePoint& point : visible) {
            const float pointValue = point.getValue();
            if (isnan(pointValue)) continue;
            
            // Update value bounds only
            if (pointValue < newValueLower) newValueLower = pointValue;
            if (pointValue > newValueUpper) newValueUpper = pointValue;
        }
        
        // Only update if we found valid values
        if (newValueLower != numeric_limits<float>::infinity()) {
            valueLower = newValueLower;
            valueUpper = newValueUpper;
            DBG("Chart::fitToVisiblePoints() - new Y bounds: valueLower=" + to_string(valueLower) + ", valueUpper=" + to_string(valueUpper));
        } else {
            DBG("Chart::fitToVisiblePoints() - no valid point values found");
        }
    }

    // Check if data bounds are valid (valueLast > valueFirst)
    bool hasValidDataBounds() const {
        return valueLast > valueFirst && valueFirst > 0 && valueLast > 0;
    }
    
    // Check if view bounds are valid (viewLast > viewFirst)
    bool hasValidViewBounds() const {
        return viewLast > viewFirst && viewFirst > 0 && viewLast > 0;
    }
    
    // Convert pixel to time
    time_sec pixelToTime(int pixelX) const {
        if (!hasValidDataBounds()) return valueFirst;
        
        double ratio = (double)(pixelX - spacingLeft) / innerWidth();
        return valueFirst + (time_sec)(ratio * (valueLast - valueFirst));
    }

    // Zoom at pixel position (factor > 1 = zoom in, factor < 1 = zoom out)
    void zoomAt(double factor, int pixelX) {
        // Validate bounds first
        DBG("Chart::zoomAt() - entry: valueFirst=" + to_string(valueFirst) + ", valueLast=" + to_string(valueLast) + ", viewFirst=" + to_string(viewFirst) + ", viewLast=" + to_string(viewLast) + ", viewInitialized=" + to_string(viewInitialized));
        if (!hasValidDataBounds()) {
            DBG("Chart::zoomAt() - invalid data bounds, skipping");
            return;
        }
        if (!hasValidViewBounds()) {
            DBG("Chart::zoomAt() - invalid view bounds, skipping");
            return;
        }
        
        time_sec visibleDuration = viewLast - viewFirst;
        time_sec dataDuration = valueLast - valueFirst;
        
        DBG("Chart::zoomAt() - factor=" + to_string(factor) + ", visibleDuration=" + to_string(visibleDuration) + ", dataDuration=" + to_string(dataDuration));
        
        time_sec newDuration = (time_sec)(visibleDuration / factor);
        
        // Clamp to data boundaries - max zoom out when all data is visible
        if (newDuration > dataDuration) newDuration = dataDuration;
        
        // Calculate relative X position (0.0 to 1.0)
        double relativeX = (double)(pixelX - spacingLeft) / innerWidth();
        
        // Guard against invalid relativeX
        if (relativeX < 0.0 || relativeX > 1.0) {
            DBG("Chart::zoomAt() - invalid relativeX=" + to_string(relativeX) + ", clamping to 0.5");
            relativeX = 0.5;
        }
        
        // Edge quarter logic: stick to edges
        if (relativeX < 0.25) {
            // Stick to left edge
            viewFirst = valueFirst;
            viewLast = viewFirst + newDuration;
        } else if (relativeX > 0.75) {
            // Stick to right edge
            viewLast = valueLast;
            viewFirst = viewLast - newDuration;
        } else {
            // Center on pixel position
            time_sec centerTime = valueFirst + (time_sec)(relativeX * dataDuration);
            
            // Guard against overflow in newDuration * relativeX
            double newDurationDouble = (double)newDuration;
            double offsetDouble = newDurationDouble * relativeX;
            if (offsetDouble > (double)numeric_limits<time_sec>::max() || offsetDouble < (double)numeric_limits<time_sec>::min()) {
                DBG("Chart::zoomAt() - overflow risk in offset calculation, using simplified centering");
                viewFirst = centerTime - newDuration / 2;
                viewLast = centerTime + newDuration / 2;
            } else {
                viewFirst = centerTime - (time_sec)offsetDouble;
                viewLast = viewFirst + newDuration;
            }
            
            // Clamp to data boundaries (only right side needs checking in middle case)
            if (viewLast > valueLast) {
                viewLast = valueLast;
                viewFirst = viewLast - newDuration;
            }
        }
        
        DBG("Chart::zoomAt() - new view: [" + to_string(viewFirst) + ", " + to_string(viewLast) + "]");
    }

    // Scroll by delta in pixels (mobile-style: drag left moves chart right)
    void scrollBy(double deltaPixels) {
        if (!hasValidDataBounds() || !hasValidViewBounds()) return;
        
        // Calculate seconds per pixel based on FULL data range
        double secondsPerPixel = (double)(valueLast - valueFirst) / innerWidth();
        time_sec deltaTime = (time_sec)(deltaPixels * secondsPerPixel);
        
        viewFirst += deltaTime;
        viewLast += deltaTime;
        
        // Clamp to data boundaries
        if (viewFirst < valueFirst) viewFirst = valueFirst;
        if (viewLast > valueLast) viewLast = valueLast;
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
        unsigned int color = CHART_COLOR_PLOTTER //,
        // double spacing = 0.1 // TODO give width for the bars somehow!
    ) {
        // If we don't have a valid time range or drawable width, bail out
        if (points.size() < 2) return;
        if (!hasValidDataBounds()) return;
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
        if (!hasValidDataBounds()) return;
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
    virtual bool showLine(
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
    // Uses view window for visible data
    int timeToX(time_sec time) const {
        if (valueLast <= valueFirst) return innerWidth() / 2; // Avoid division by zero
        
        // Use view window if initialized, otherwise use full data range
        time_sec viewStart = viewInitialized ? viewFirst : valueFirst;
        time_sec viewEnd = viewInitialized ? viewLast : valueLast;
        
        if (viewEnd <= viewStart) return innerWidth() / 2;
        
        // Linear interpolation: map [viewFirst, viewLast] to [spacingLeft, canvas.width()-spacingRight]
        double ratio = (double)(time - viewStart) / (viewEnd - viewStart);
        int x = (int)(ratio * innerWidth());
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
    time_sec viewFirst;
    time_sec viewLast;
    bool viewInitialized = false;

protected:
    double zoomInFactor;
    double zoomOutFactor;
};
