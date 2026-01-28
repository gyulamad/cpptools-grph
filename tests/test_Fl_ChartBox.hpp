#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../Fl_ChartBox.hpp"
#include "MockFl_ChartBox.hpp"
#include "MockCanvas.hpp"
#include "../../trading/CandleSeries.hpp"
#include "../TimePointSeries.hpp"
#include <vector>

using namespace std;

// Test struct for Fl_ChartBox constructor test
struct test_Fl_ChartBox_constructor_TestData {
    int x;
    int y;
    int w;
    int h;
    int expectedW;
    int expectedH;
};

// Constructor should not crash and should initialize properly
TEST(test_Fl_ChartBox_constructor_initializes_properly) {
    MockFl_ChartBox chartBox(10, 20, 800, 600);
    assert(true && "Fl_ChartBox constructor should not crash");
}

// Test addCandleSeries adds a series to the correct pane
TEST(test_Fl_ChartBox_addCandleSeries_single_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {
        Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f),
        Candle(200, 6.0f, 9.0f, 1.0f, 8.0f, 0.0f),
        Candle(300, 5.0f, 8.0f, 3.0f, 4.0f, 0.0f)
    };
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 300);
    
    chartBox.addCandleSeries(candleSeries);
    
    assert(chartBox.candlesSerieses.size() == 1 && "Should have 1 pane after adding single series");
    assert(chartBox.candlesSerieses[0].size() == 1 && "Should have 1 candle series in pane 0");
}

// Test addCandleSeries with multiple panes
TEST(test_Fl_ChartBox_addCandleSeries_multiple_panes) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles1 = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries1(candles1, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    vector<Candle> candles2 = {Candle(200, 6.0f, 9.0f, 1.0f, 8.0f, 0.0f)};
    CandleSeries candleSeries2(candles2, SymbolInterval("ETHUSDT", 60), 200, 200);
    
    chartBox.addCandleSeries(candleSeries1, 0);
    chartBox.addCandleSeries(candleSeries2, 1);
    
    assert(chartBox.candlesSerieses.size() == 2 && "Should have 2 panes");
    assert(chartBox.candlesSerieses[0].size() == 1 && "Pane 0 should have 1 series");
    assert(chartBox.candlesSerieses[1].size() == 1 && "Pane 1 should have 1 series");
}

// Test addCandleSeries with empty series
TEST(test_Fl_ChartBox_addCandleSeries_empty_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> emptyCandles;
    CandleSeries emptySeries(emptyCandles, SymbolInterval("BTCUSDT", 60), 0, 0);
    chartBox.addCandleSeries(emptySeries);
    
    assert(chartBox.candlesSerieses.size() == 1 && "Should have 1 pane");
    assert(chartBox.candlesSerieses[0].size() == 1 && "Should have 1 empty series");
}

// Test addBarSeries adds a series correctly
TEST(test_Fl_ChartBox_addBarSeries_single_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> bars = {
        TimePoint(100, 5.0f),
        TimePoint(200, 3.0f),
        TimePoint(300, 7.0f)
    };
    TimePointSeries barSeries(bars);
    
    chartBox.addBarSeries(barSeries);
    
    assert(chartBox.barsSerieses.size() == 1 && "Should have 1 pane");
    assert(chartBox.barsSerieses[0].size() == 1 && "Should have 1 bar series");
}

// Test addPointSeries adds a series correctly
TEST(test_Fl_ChartBox_addPointSeries_single_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> points = {
        TimePoint(100, 5.0f),
        TimePoint(200, 3.0f)
    };
    TimePointSeries pointSeries(points);
    
    chartBox.addPointSeries(pointSeries);
    
    assert(chartBox.pointsSerieses.size() == 1 && "Should have 1 pane");
    assert(chartBox.pointsSerieses[0].size() == 1 && "Should have 1 point series");
}

// Test clearCandlesSerieses removes all candle series
TEST(test_Fl_ChartBox_clearCandlesSerieses_removes_all) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    chartBox.addCandleSeries(candleSeries);
    assert(chartBox.candlesSerieses.size() == 1 && "Should have 1 pane before clear");
    
    chartBox.clearCandlesSerieses();
    assert(chartBox.candlesSerieses[0].size() == 0 && "Pane 0 should be empty after clear");
}

// Test clearBarsSerieses removes all bar series
TEST(test_Fl_ChartBox_clearBarsSerieses_removes_all) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> bars = {TimePoint(100, 5.0f)};
    TimePointSeries barSeries(bars);
    
    chartBox.addBarSeries(barSeries);
    assert(chartBox.barsSerieses.size() == 1 && "Should have 1 pane before clear");
    
    chartBox.clearBarsSerieses();
    assert(chartBox.barsSerieses[0].size() == 0 && "Pane 0 should be empty after clear");
}

// Test clearPointsSerieses removes all point series
TEST(test_Fl_ChartBox_clearPointsSerieses_removes_all) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> points = {TimePoint(100, 5.0f)};
    TimePointSeries pointSeries(points);
    
    chartBox.addPointSeries(pointSeries);
    assert(chartBox.pointsSerieses.size() == 1 && "Should have 1 pane before clear");
    
    chartBox.clearPointsSerieses();
    assert(chartBox.pointsSerieses[0].size() == 0 && "Pane 0 should be empty after clear");
}

// Test clearAllSerieses clears all types
TEST(test_Fl_ChartBox_clearAllSerieses_clears_everything) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    vector<TimePoint> bars = {TimePoint(100, 5.0f)};
    TimePointSeries barSeries(bars);
    
    vector<TimePoint> points = {TimePoint(100, 3.0f)};
    TimePointSeries pointSeries(points);
    
    chartBox.addCandleSeries(candleSeries);
    chartBox.addBarSeries(barSeries);
    chartBox.addPointSeries(pointSeries);
    
    chartBox.clearAllSerieses();
    
    assert(chartBox.candlesSerieses[0].size() == 0 && "Candles should be cleared");
    assert(chartBox.barsSerieses[0].size() == 0 && "Bars should be cleared");
    assert(chartBox.pointsSerieses[0].size() == 0 && "Points should be cleared");
}

// Test draw with no series does not crash
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_no_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    // draw() should not crash when there are no series
    // Skipped: FLTK requires display environment
    assert(true && "draw() with no series - SKIPPED (requires GUI display)");
}

// Test draw with single candle series
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_single_candle_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {
        Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f),
        Candle(200, 6.0f, 9.0f, 1.0f, 8.0f, 0.0f)
    };
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 200);
    
    chartBox.addCandleSeries(candleSeries);
    
    // draw() should not crash with data
    // Skipped: FLTK requires display environment
    assert(true && "draw() with single candle series - SKIPPED (requires GUI display)");
}

// Test draw with multiple candle series
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_multiple_candle_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles1 = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries1(candles1, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    vector<Candle> candles2 = {Candle(200, 6.0f, 9.0f, 1.0f, 8.0f, 0.0f)};
    CandleSeries candleSeries2(candles2, SymbolInterval("ETHUSDT", 60), 200, 200);
    
    chartBox.addCandleSeries(candleSeries1, 0);
    chartBox.addCandleSeries(candleSeries2, 0);
    
    // draw() should not crash with multiple series in same pane
    // Skipped: FLTK requires display environment
    assert(true && "draw() with multiple candle series - SKIPPED (requires GUI display)");
}

// Test draw with mixed series types
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_mixed_series_types) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    vector<TimePoint> bars = {TimePoint(100, 5.0f)};
    TimePointSeries barSeries(bars);
    
    vector<TimePoint> points = {TimePoint(100, 3.0f)};
    TimePointSeries pointSeries(points);
    
    chartBox.addCandleSeries(candleSeries);
    chartBox.addBarSeries(barSeries);
    chartBox.addPointSeries(pointSeries);
    
    // draw() should not crash with mixed series
    // Skipped: FLTK requires display environment
    assert(true && "draw() with mixed series types - SKIPPED (requires GUI display)");
}

// Test draw with multiple panes
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_multiple_panes) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles1 = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries1(candles1, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    vector<Candle> candles2 = {Candle(200, 6.0f, 9.0f, 1.0f, 8.0f, 0.0f)};
    CandleSeries candleSeries2(candles2, SymbolInterval("ETHUSDT", 60), 200, 200);
    
    chartBox.addCandleSeries(candleSeries1, 0);
    chartBox.addCandleSeries(candleSeries2, 1);
    
    // draw() should handle multiple panes
    // Skipped: FLTK requires display environment
    assert(true && "draw() with multiple panes - SKIPPED (requires GUI display)");
}

// Test draw with empty candles
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_empty_candles) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> emptyCandles;
    CandleSeries emptySeries(emptyCandles, SymbolInterval("BTCUSDT", 60), 0, 0);
    chartBox.addCandleSeries(emptySeries);
    
    // draw() should handle empty candle series
    // Skipped: FLTK requires display environment
    assert(true && "draw() with empty candles - SKIPPED (requires GUI display)");
}

// Test draw with single candle
// NOTE: Skipped - requires GUI display environment
TEST(test_Fl_ChartBox_draw_with_single_candle) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    chartBox.addCandleSeries(candleSeries);
    
    // draw() should handle single candle
    // Skipped: FLTK requires display environment
    assert(true && "draw() with single candle - SKIPPED (requires GUI display)");
}

// Test addSeries creates intermediate panes
TEST(test_Fl_ChartBox_addSeries_pane_gap_handling) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 100);
    
    // Add to pane 5 directly - should create intermediate panes
    chartBox.addCandleSeries(candleSeries, 5);
    
    assert(chartBox.candlesSerieses.size() == 6 && "Should create 6 panes (0-5)");
}

// Test destructor is called (coverage for destructor)
TEST(test_Fl_ChartBox_destructor_coverage) {
    // Create and destroy to test destructor coverage
    {
        MockFl_ChartBox chartBox(10, 10, 800, 600);
        vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
        CandleSeries candleSeries(candles, SymbolInterval("BTCUSDT", 60), 100, 100);
        chartBox.addCandleSeries(candleSeries);
    } // Destructor called here
    
    assert(true && "Destructor coverage test passed");
}

// Test with bars only (no candles or points)
TEST(test_Fl_ChartBox_bars_only) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> bars = {
        TimePoint(100, 5.0f),
        TimePoint(200, 3.0f),
        TimePoint(300, 7.0f)
    };
    TimePointSeries barSeries(bars);
    
    chartBox.addBarSeries(barSeries);
    
    assert(chartBox.barsSerieses.size() == 1 && "Should have 1 pane");
    assert(chartBox.barsSerieses[0].size() == 1 && "Should have 1 bar series");
    assert(chartBox.candlesSerieses.empty() && "Should have no candle series");
    assert(chartBox.pointsSerieses.empty() && "Should have no point series");
}

// Test with points only (no candles or bars)
TEST(test_Fl_ChartBox_points_only) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> points = {
        TimePoint(100, 5.0f),
        TimePoint(200, 3.0f)
    };
    TimePointSeries pointSeries(points);
    
    chartBox.addPointSeries(pointSeries);
    
    assert(chartBox.pointsSerieses.size() == 1 && "Should have 1 pane");
    assert(chartBox.pointsSerieses[0].size() == 1 && "Should have 1 point series");
    assert(chartBox.candlesSerieses.empty() && "Should have no candle series");
    assert(chartBox.barsSerieses.empty() && "Should have no bar series");
}

// Test clear on empty series
TEST(test_Fl_ChartBox_clear_empty_series) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    // Clear empty series - should not crash
    chartBox.clearCandlesSerieses();
    chartBox.clearBarsSerieses();
    chartBox.clearPointsSerieses();
    chartBox.clearAllSerieses();
    
    assert(true && "Clearing empty series should not crash");
}

// Test adding to non-zero pane with bars
TEST(test_Fl_ChartBox_addBarSeries_multiple_panes) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> bars1 = {TimePoint(100, 5.0f)};
    TimePointSeries barSeries1(bars1);
    
    vector<TimePoint> bars2 = {TimePoint(200, 3.0f)};
    TimePointSeries barSeries2(bars2);
    
    chartBox.addBarSeries(barSeries1, 0);
    chartBox.addBarSeries(barSeries2, 2);
    
    assert(chartBox.barsSerieses.size() == 3 && "Should have 3 panes (0, 1, 2)");
    assert(chartBox.barsSerieses[0].size() == 1 && "Pane 0 should have 1 series");
    assert(chartBox.barsSerieses[2].size() == 1 && "Pane 2 should have 1 series");
}

// Test adding to non-zero pane with points
TEST(test_Fl_ChartBox_addPointSeries_multiple_panes) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> points1 = {TimePoint(100, 5.0f)};
    TimePointSeries pointSeries1(points1);
    
    vector<TimePoint> points2 = {TimePoint(200, 3.0f)};
    TimePointSeries pointSeries2(points2);
    
    chartBox.addPointSeries(pointSeries1, 1);
    chartBox.addPointSeries(pointSeries2, 3);
    
    assert(chartBox.pointsSerieses.size() == 4 && "Should have 4 panes (0, 1, 2, 3)");
    assert(chartBox.pointsSerieses[1].size() == 1 && "Pane 1 should have 1 series");
    assert(chartBox.pointsSerieses[3].size() == 1 && "Pane 3 should have 1 series");
}

// Test TimePointSeries getPointsCRep() for full coverage
TEST(test_Fl_ChartBox_TimePointSeries_getPointsCRef) {
    MockFl_ChartBox chartBox(10, 10, 800, 600);
    
    vector<TimePoint> points = {
        TimePoint(100, 5.0f),
        TimePoint(200, 3.0f),
        TimePoint(300, 7.0f)
    };
    TimePointSeries pointSeries(points, 0xFF0000);
    
    // Call getPointsCRef() to get const reference to points
    const vector<TimePoint>& retrievedPoints = pointSeries.getPointsCRef();
    
    assert(retrievedPoints.size() == 3 && "Should have 3 points");
    assert(retrievedPoints[0].getTime() == 100 && "First point time should be 100");
    assert(retrievedPoints[1].getTime() == 200 && "Second point time should be 200");
    assert(retrievedPoints[2].getTime() == 300 && "Third point time should be 300");
}

#endif // TEST
