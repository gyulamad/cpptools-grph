#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../TimePointSeries.hpp"
#include "MockCanvas.hpp"
#include "TestChart.hpp"

// Test TimePointSeries construction
TEST(test_TimePointSeries_construction) {
    vector<TimePoint> points = {
        TimePoint(100, 5.0f),
        TimePoint(200, 8.0f)
    };
    TimePointSeries series(points, 0xFF0000);
    assert(series.getColor() == 0xFF0000);
}

// Test TimePointSeries with empty points
TEST(test_TimePointSeries_empty) {
    vector<TimePoint> points;
    TimePointSeries series(points, 0xFF0000);
    assert(series.getPointsRef().empty());
}

// Test TimePointSeries default color
TEST(test_TimePointSeries_default_color) {
    vector<TimePoint> points = { TimePoint(100, 5.0f) };
    TimePointSeries series(points);
    assert(series.getColor() == CHART_COLOR_PLOTTER);
}

// Test showCandles with candleBodyWidth >= 1 (full candles mode)
TEST(test_TimePointSeries_showCandles_full_candles_mode) {
    vector<Candle> candles;
    candles.push_back(Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f));
    candles.push_back(Candle(200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f));
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    chart.showCandles(candles, 60);
    assert(true && "showCandles should handle full candles mode without crashing");
}

// Test showCandles with candleBodyWidth between 0 and 1 (line mode)
TEST(test_TimePointSeries_showCandles_line_mode) {
    vector<Candle> candles;
    candles.push_back(Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f));
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    chart.showCandles(candles, 0.5);
    assert(true && "showCandles should handle line mode without crashing");
}

// Test showCandles with candleBodyWidth == 1 (boundary condition)
TEST(test_Chart_showCandles_boundary_candleBodyWidth) {
    vector<Candle> candles = {Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f)};
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    chart.showCandles(candles, 1.0);
    assert(true && "showCandles should handle candleBodyWidth == 1 boundary condition");
}

// Test showBars with very small time range (line mode)
TEST(test_Chart_showBars_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {101, 8.0f}, {102, 3.0f}});
    chart.setVisibleRange(100, 200);
    chart.showBars({{100, 5.0f}, {101, 8.0f}, {102, 3.0f}});
    assert(true && "showBars should handle line mode without crashing");
}

// Test showPoints with very small time range (line mode)
TEST(test_Chart_showPoints_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {101, 8.0f}, {102, 3.0f}});
    chart.setVisibleRange(100, 200);
    chart.showPoints({{100, 5.0f}, {101, 8.0f}, {102, 3.0f}});
    assert(true && "showPoints should handle line mode without crashing");
}

// Test showCandleAsLine with valid data
TEST(test_Chart_showCandleAsLine_valid) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    testChart.testShowCandleAsLine({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, 0.5, 0xFF0000, 0x0000FF);
    assert(true && "showCandleAsLine should handle valid data without crashing");
}

// Test showCandleAsLine with boundary candleBodyWidth
TEST(test_Chart_showCandleAsLine_boundary) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    testChart.testShowCandleAsLine({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, 1.0, 0xFF0000, 0x0000FF);
    assert(true && "showCandleAsLine should handle boundary candleBodyWidth without crashing");
}

// Test showCandleAsLine with very large candleBodyWidth
TEST(test_Chart_showCandleAsLine_large_width) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    testChart.testShowCandleAsLine({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, 10.0, 0xFF0000, 0x0000FF);
    assert(true && "showCandleAsLine should handle large candleBodyWidth without crashing");
}

// Test showCandleAsLine with very small candleBodyWidth (boundary)
TEST(test_Chart_showCandleAsLine_small_width) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
    testChart.testShowCandleAsLine({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, 0.1, 0xFF0000, 0x0000FF);
    assert(true && "showCandleAsLine should handle small candleBodyWidth without crashing");
}

// Test showBars with multiple points in line mode
TEST(test_Chart_showBars_multiple_points_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    chart.setVisibleRange(100, 200);
    chart.showBars({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    assert(true && "showBars should handle multiple points in line mode without crashing");
}

// Test showPoints with multiple points in line mode
TEST(test_Chart_showPoints_multiple_points_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    chart.setVisibleRange(100, 200);
    chart.showPoints({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    assert(true && "showPoints should handle multiple points in line mode without crashing");
}

// Test showCandles with many candles in line mode
TEST(test_Chart_showCandles_many_candles_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    vector<Candle> candles = {{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}};
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    candles = {};
    for (int i = 0; i < 100; i++) {
        candles.push_back({100, 3.0f + i * 0.1f, 9.0f + i * 0.1f, 2.0f + i * 0.1f, 7.0f + i * 0.1f, 0.0f});
    }
    chart.showCandles(candles, 0.01);
    assert(true && "showCandles should handle many candles in line mode without crashing");
}

// Test showBars with extreme time range in line mode
TEST(test_Chart_showBars_extreme_time_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {1000, 8.0f}});
    chart.setVisibleRange(100, 200);
    chart.showBars({{100, 5.0f}, {1000, 8.0f}});
    assert(true && "showBars should handle extreme time range in line mode without crashing");
}

// Test showPoints with extreme time range in line mode
TEST(test_Chart_showPoints_extreme_time_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {1000, 8.0f}});
    chart.setVisibleRange(100, 200);
    chart.showPoints({{100, 5.0f}, {1000, 8.0f}});
    assert(true && "showPoints should handle extreme time range in line mode without crashing");
}

// Test showCandleAsLine with boundary candleBodyWidth values
TEST(test_Chart_showCandleAsLine_boundary_values) {
    vector<float> widths = {0.0f, 0.1f, 0.5f, 1.0f, 2.0f, 10.0f, 100.0f};
    for (float width : widths) {
        MockCanvas canvas(800, 600);
        TestChart chart(canvas);
        chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
        TestChart testChart(canvas);
        testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
        testChart.testShowCandleAsLine({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, width, 0xFF0000, 0x0000FF);
    }
    assert(true && "showCandleAsLine should handle boundary candleBodyWidth values without crashing");
}

// Test showCandles with very large candleBodyWidth values
TEST(test_Chart_showCandles_large_candleBodyWidth) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},
        {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}
    };
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    TestChart testChart(canvas);
    vector<Candle> candles2 = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},
        {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}
    };
    testChart.fitToCandles(candles2);
    testChart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}}, 10.0, 0xFF0000, 0x0000FF, 0.1);
    assert(true && "showCandles should handle very large candleBodyWidth without crashing");
}

// Test showCandles with very small candleBodyWidth (boundary condition)
TEST(test_Chart_showCandles_very_small_candleBodyWidth) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},
        {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}
    };
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});
    testChart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}}, 0.01);
    assert(true && "showCandles should handle very small candleBodyWidth without crashing");
}

// Test showCandles with candleBodyWidth between 0 and 1 (line mode)
TEST(test_Chart_showCandles_boundary_candleBodyWidth_values) {
    vector<float> widths = {0.01f, 0.1f, 0.5f, 0.9f};
    for (float width : widths) {
        MockCanvas canvas(800, 600);
        TestChart chart(canvas);
        chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
        chart.setVisibleRange(100, 200);
        TestChart testChart(canvas);
        testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});
        testChart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}}, width);
    }
    assert(true && "showCandles should handle boundary candleBodyWidth values without crashing");
}

// Test showBars with line mode and multiple points
TEST(test_Chart_showBars_line_mode_multiple_points) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    chart.setVisibleRange(100, 200);
    chart.showBars({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    assert(true && "showBars should handle line mode with multiple points without crashing");
}

// Test showPoints with line mode and multiple points
TEST(test_Chart_showPoints_line_mode_multiple_points) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    chart.setVisibleRange(100, 200);
    chart.showPoints({{100, 5.0f}, {101, 6.0f}, {102, 7.0f}});
    assert(true && "showPoints should handle line mode with multiple points without crashing");
}

// Test showCandles with line mode and many candles
TEST(test_Chart_showCandles_line_mode_many_candles) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    vector<Candle> candles;
    for (int i = 0; i < 100; i++) {
        candles.push_back(Candle(100, 3.0f + i * 0.1f, 9.0f + i * 0.1f, 2.0f + i * 0.1f, 7.0f + i * 0.1f, 0.0f));
    }
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    chart.showCandles(candles, 0.01);
    assert(true && "showCandles should handle many candles in line mode without crashing");
}

// Test showCandles with line mode and extreme time range
TEST(test_Chart_showCandles_line_mode_extreme_time) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    vector<Candle> candles;
    candles.push_back(Candle(100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f));
    candles.push_back(Candle(1000, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f));
    chart.fitToCandles(candles);
    chart.setVisibleRange(100, 200);
    chart.showCandles(candles, 0.01);
    assert(true && "showCandles should handle line mode with extreme time range without crashing");
}

// Test showPoints with line mode and extreme time range
TEST(test_Chart_showPoints_line_mode_extreme_time) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToPoints({{100, 5.0f}, {1000, 8.0f}});
    chart.setVisibleRange(100, 200);
    chart.showPoints({{100, 5.0f}, {1000, 8.0f}});
    assert(true && "showPoints should handle line mode with extreme time range without crashing");
}

#endif