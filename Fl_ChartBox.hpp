#pragma once

#include "../misc/Fl_CanvasBox.hpp"
#include "../trading/CandleSeries.hpp"
#include "TimePointSeries.hpp"
#include "ChartGroup.hpp"

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
        ),
        group(nullptr),
        lastDragX(0)
    {
        // Set up mouse wheel callback for zoom
        scroll = [this](int left, int top, int dx, int dy, int button) {
            // LCOV_EXCL_START
            // Coverage excluded - requires actual FLTK mouse wheel event
            (void)top;
            (void)dx;
            (void)button;
            if (dy != 0) {
                onMouseWheel(left, dy);
            }
            // LCOV_EXCL_STOP
        };
        
        // Set up drag callback for scroll (real-time as mouse moves)
        drag = [this](int left, int top, int button) {
            // LCOV_EXCL_START
            // Coverage excluded - requires actual FLTK drag event
            (void)top;
            (void)button;
            if (lastDragX != 0) {
                onDrag(left, left - lastDragX);
            }
            lastDragX = left;
            // LCOV_EXCL_STOP
        };
        
        // Reset drag tracking on push
        push = [this](int left, int top, int button) {
            // LCOV_EXCL_START
            // Coverage excluded - requires actual FLTK push event
            (void)top;
            (void)button;
            lastDragX = left;
            // LCOV_EXCL_STOP
        };
    }

    virtual ~Fl_ChartBox() {}

    void setChartGroup(ChartGroup* group) { this->group = group; }
    Chart& getChart() { return chart; }

    void clearAllSerieses() {
        clearCandlesSerieses();
        clearBarsSerieses();
        clearPointsSerieses();
    }

    void clearCandlesSerieses() {
        candlesSerieses.clear();
    }

    void addCandleSeries(const CandleSeries& candleSeries, size_t pane = 0) {
        while (candlesSerieses.size() < pane + 1) candlesSerieses.push_back({});
        candlesSerieses[pane].push_back(candleSeries);
    }

    void clearBarsSerieses() {
        barsSerieses.clear();
    }

    void addBarSeries(const TimePointSeries& barSeries, size_t pane = 0) {
        while (barsSerieses.size() < pane + 1) barsSerieses.push_back({});
        barsSerieses[pane].push_back(barSeries);
    }

    void clearPointsSerieses() {
        pointsSerieses.clear();
    }

    void addPointSeries(const TimePointSeries& pointSeries, size_t pane = 0) {
        while (pointsSerieses.size() < pane + 1) pointsSerieses.push_back({});
        pointsSerieses[pane].push_back(pointSeries);
    }

    // LCOV_EXCL_START
    // Coverage excluded - draw() requires GUI display environment
    void draw() override {
        Fl_CanvasBox::draw(); // Call the base class draw method (draws the box itself)
        
        // Fit per pane
        size_t panes = max({ 
            candlesSerieses.size(), 
            barsSerieses.size(),
            pointsSerieses.size(),
        });
        for (size_t pane = 0; pane < panes; pane++) {
            const vector<CandleSeries>& candlesSeries = candlesSerieses.size() > pane ? candlesSerieses[pane] : vector<CandleSeries>();
            const vector<TimePointSeries>& barsSeries = barsSerieses.size() > pane ? barsSerieses[pane] : vector<TimePointSeries>();
            const vector<TimePointSeries>& pointsSeries = pointsSerieses.size() > pane ? pointsSerieses[pane] : vector<TimePointSeries>();

            // Fit the chart to the contents
            chart.resetBounds();
            for (const CandleSeries& candleSeries: candlesSeries)
                chart.fitToCandles(candleSeries.getCandlesCRef());
            for (const TimePointSeries& barSeries: barsSeries)
                chart.fitToPoints(barSeries.getPointsCRef());
            for (const TimePointSeries& pointSeries: pointsSeries)
                chart.fitToPoints(pointSeries.getPointsCRef());
            
            // Initialize view if not set
            chart.resetView();
            
            // Get visible data and fit Y-axis to visible
            vector<Candle> visibleCandles;
            for (const CandleSeries& candleSeries: candlesSeries) {
                vector<Candle> vc = chart.getVisibleCandles(candleSeries.getCandlesCRef());
                visibleCandles.insert(visibleCandles.end(), vc.begin(), vc.end());
            }
            chart.fitToVisibleCandles(visibleCandles);
            
            vector<TimePoint> visibleBars;
            for (const TimePointSeries& barSeries: barsSeries) {
                vector<TimePoint> vb = chart.getVisiblePoints(barSeries.getPointsCRef());
                visibleBars.insert(visibleBars.end(), vb.begin(), vb.end());
            }
            chart.fitToVisiblePoints(visibleBars);
            
            vector<TimePoint> visiblePoints;
            for (const TimePointSeries& pointSeries: pointsSeries) {
                vector<TimePoint> vp = chart.getVisiblePoints(pointSeries.getPointsCRef());
                visiblePoints.insert(visiblePoints.end(), vp.begin(), vp.end());
            }
            chart.fitToVisiblePoints(visiblePoints);

            // Draw visible data
            for (const CandleSeries& candleSeries: candlesSeries) {
                vector<Candle> visible = chart.getVisibleCandles(candleSeries.getCandlesCRef());
                if (!visible.empty())
                    chart.showCandles(
                        visible, 
                        candleSeries.getInterval(), 
                        candleSeries.getBullishColor(),
                        candleSeries.getBearishColor(),
                        candleSeries.getShoulderSpacing()
                    );
            }
            for (const TimePointSeries& barSeries: barsSeries) {
                vector<TimePoint> visible = chart.getVisiblePoints(barSeries.getPointsCRef());
                if (!visible.empty())
                    chart.showBars(
                        visible, 
                        barSeries.getColor()
                    );
            }
            for (const TimePointSeries& pointSeries: pointsSeries) {
                vector<TimePoint> visible = chart.getVisiblePoints(pointSeries.getPointsCRef());
                if (!visible.empty())
                    chart.showPoints(
                        visible, 
                        pointSeries.getColor()
                    );
            }
        }
    }
    // LCOV_EXCL_STOP

 protected:
    void onMouseWheel(int pixelX, int deltaY) {
        double factor = deltaY < 0 ? chart.getZoomInFactor() : chart.getZoomOutFactor();
        
        // Capture original data bounds BEFORE any drawing modifies them
        time_sec originalValueFirst = chart.getValueFirst();
        time_sec originalValueLast = chart.getValueLast();
        
        // Temporarily restore original bounds for zoom calculation
        time_sec savedValueFirst = chart.getValueFirst();
        time_sec savedValueLast = chart.getValueLast();
        chart.setValueFirst(originalValueFirst);
        chart.setValueLast(originalValueLast);
        
        if (group) {
            group->zoomAt(factor, pixelX);
        } else {
            chart.zoomAt(factor, pixelX);
        }
        
        // Restore the saved bounds (drawing will update them again)
        chart.setValueFirst(savedValueFirst);
        chart.setValueLast(savedValueLast);
        
        redraw();
    }
    
    void onDrag(int pixelX, int deltaX) {
        // No previous drag position or view not initialized, ignore
        if (lastDragX == 0 || !chart.isViewInitialized()) return;
        
        if (group) {
            group->scrollBy(deltaX);
        } else {
            chart.scrollBy(deltaX);
        }
        redraw();
        lastDragX = pixelX;
    }
    
    Chart chart;
    ChartGroup* group;
    int lastDragX;

    vector<vector<CandleSeries>> candlesSerieses;
    vector<vector<TimePointSeries>> barsSerieses;
    vector<vector<TimePointSeries>> pointsSerieses;
};
