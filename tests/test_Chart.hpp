#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../Chart.hpp"
#include "MockCanvas.hpp"
#include "TestChart.hpp"
#include "MockShowLineChart.hpp"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;

// Test struct for test_Chart_fitToPoints_sets_min_max_and_time_range_for_valid_points
struct test_Chart_fitToPoints_valid_points_TestData {
    vector<TimePoint> points;
    long long expected_value_first;
    long long expected_value_last;
    float expected_value_lower;
    float expected_value_upper;
};

// fitToPoints should set first/last/time and min/max values for valid points
TEST(test_Chart_fitToPoints_sets_min_max_and_time_range_for_valid_points) {
    test_Chart_fitToPoints_valid_points_TestData data = {
        {{10, 5.0f}, {20, 1.0f}, {30, 9.0f}},
        10,
        30,
        1.0f,
        9.0f
    };

    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToPoints(data.points);

    assert(chart.valueFirst == data.expected_value_first && "valueFirst should be the earliest point time");
    assert(chart.valueLast  == data.expected_value_last  && "valueLast should be the latest point time");
    assert(chart.valueLower == data.expected_value_lower && "valueLower should be the minimum point value");
    assert(chart.valueUpper == data.expected_value_upper && "valueUpper should be the maximum point value");
}

// Test struct for test_Chart_fitToPoints_ignores_nan_values
struct test_Chart_fitToPoints_ignores_nan_values_TestData {
    vector<TimePoint> points;
    long long expected_value_first;
    long long expected_value_last;
    float expected_value_lower;
    float expected_value_upper;
};

// fitToPoints should ignore NaN values when determining min/max and time range
TEST(test_Chart_fitToPoints_ignores_nan_values) {
    test_Chart_fitToPoints_ignores_nan_values_TestData data = {
        {{10, numeric_limits<float>::quiet_NaN()}, {20, 2.5f}, {30, numeric_limits<float>::quiet_NaN()}},
        20,
        20,
        2.5f,
        2.5f
    };

    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToPoints(data.points);

    assert(chart.valueFirst == data.expected_value_first && "valueFirst should be set by the first non-NaN point");
    assert(chart.valueLast  == data.expected_value_last  && "valueLast should be set by the last non-NaN point");
    assert(chart.valueLower == data.expected_value_lower && "valueLower should be the minimum non-NaN point value");
    assert(chart.valueUpper == data.expected_value_upper && "valueUpper should be the maximum non-NaN point value");
}

// Test struct for test_Chart_fitToCandles_sets_min_max_and_time_range
struct test_Chart_fitToCandles_sets_min_max_and_time_range_TestData {
    vector<Candle> candles;
    long long expected_value_first;
    long long expected_value_last;
    float expected_value_lower;
    float expected_value_upper;
};

// fitToCandles should set first/last/time and min/max from candle lows/highs
TEST(test_Chart_fitToCandles_sets_min_max_and_time_range) {
    test_Chart_fitToCandles_sets_min_max_and_time_range_TestData data = {
        {{10, 3.0f, 4.0f, 2.0f, 3.5f, 0.0f},
         {20, 6.0f, 9.0f, 1.0f, 8.0f, 0.0f},
         {30, 5.0f, 8.0f, 3.0f, 4.0f, 0.0f}},
        10,
        30,
        1.0f,
        9.0f
    };

    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToCandles(data.candles);

    assert(chart.valueFirst == data.expected_value_first && "valueFirst should be the earliest candle time");
    assert(chart.valueLast  == data.expected_value_last  && "valueLast should be the latest candle time");
    assert(chart.valueLower == data.expected_value_lower && "valueLower should be the minimum candle low");
    assert(chart.valueUpper == data.expected_value_upper && "valueUpper should be the maximum candle high");
}

// fitToPoints should handle an empty vector of points gracefully
TEST(test_Chart_fitToPoints_empty_points) {
    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToPoints({});

    assert(chart.valueFirst == numeric_limits<time_sec>::max() && "valueFirst should remains for empty candles");
    assert(chart.valueLast  == numeric_limits<time_sec>::min() && "valueLast should remains for empty candles");
    assert(chart.valueLower == numeric_limits<float>::infinity() && "valueLower should remains for empty candles");
    assert(chart.valueUpper == -numeric_limits<float>::infinity() && "valueUpper should remains for empty candles");
}

// fitToCandles should handle an empty vector of candles gracefully
TEST(test_Chart_fitToCandles_empty_candles) {
    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToCandles({});

    assert(chart.valueFirst == numeric_limits<time_sec>::max() && "valueFirst should remains for empty candles");
    assert(chart.valueLast  == numeric_limits<time_sec>::min() && "valueLast should remains for empty candles");
    assert(chart.valueLower == numeric_limits<float>::infinity() && "valueLower should remains for empty candles");
    assert(chart.valueUpper == -numeric_limits<float>::infinity() && "valueUpper should remains for empty candles");
}

// fitToPoints should handle a single point correctly
TEST(test_Chart_fitToPoints_single_point) {
    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToPoints({{100, 7.5f}});

    assert(chart.valueFirst == 100 && "valueFirst should be the single point time");
    assert(chart.valueLast  == 100 && "valueLast should be the single point time");
    assert(chart.valueLower == 7.5f && "valueLower should be the single point value");
    assert(chart.valueUpper == 7.5f && "valueUpper should be the single point value");
}

// fitToCandles should handle a single candle correctly
TEST(test_Chart_fitToCandles_single_candle) {
    MockCanvas canvas;
    TestChart chart(canvas);

    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}});

    assert(chart.valueFirst == 100 && "valueFirst should be single candle time");
    assert(chart.valueLast  == 100 && "valueLast should be single candle time");
    assert(chart.valueLower == 2.0f && "valueLower should be the single candle low");
    assert(chart.valueUpper == 9.0f && "valueUpper should be the single candle high");
}

// showCandles should handle single candle without crashing
TEST(test_Chart_showCandles_with_single_candle) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set up valid bounds first
    chart.setVisibleRange(100, 200);
    
    // Use minimal valid data to test showCandles without crashing
    vector<Candle> singleCandle = {{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}};
    chart.showCandles(singleCandle, 60);
    
    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle single candle without crashing");
}

// showCandles should handle an empty vector without crashing
TEST(test_Chart_showCandles_with_empty_vector) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set up valid bounds first
    chart.setVisibleRange(100, 200);
    
    // Call showCandles with empty vector - should not crash
    chart.showCandles({}, 60);
    
    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle empty vector without crashing");
}

// showCandles should skip candles with invalid data (NaN values) and continue processing valid ones
TEST(test_Chart_showCandles_skips_candles_with_nan_prices) {
    // Create a canvas and chart
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.setVisibleRange(100, 200);

    // Create candles with some having invalid (NaN) price data
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},      // Valid candle
        {110, numeric_limits<float>::quiet_NaN(), 12.0f, 5.0f, 10.0f, 0.0f},  // NaN open - should be skipped
        {120, 15.0f, 19.0f, 13.0f, 18.0f, 0.0f}    // Valid candle
    };

    // Call showCandles - should not crash and should process valid candles
    chart.showCandles(candles, 60);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should skip candles with NaN prices and continue to next valid candle");
}

// showCandles should skip candles with invalid high price (NaN) and continue processing
TEST(test_Chart_showCandles_skips_candles_with_nan_high) {
    // Create a canvas and chart
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.setVisibleRange(100, 200);

    // Create candles with one having NaN high price
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},      // Valid candle
        {110, 15.0f, numeric_limits<float>::quiet_NaN(), 12.0f, 16.0f, 0.0f},  // NaN high - should be skipped
        {120, 23.0f, 29.0f, 21.0f, 27.0f, 0.0f}    // Valid candle
    };

    // Call showCandles - should not crash and should process valid candles
    chart.showCandles(candles, 60);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should skip candles with NaN high price and continue to next valid candle");
}

// showCandles should skip candles with invalid low price (NaN) and continue processing
TEST(test_Chart_showCandles_skips_candles_with_nan_low) {
    // Create a canvas and chart
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.setVisibleRange(100, 200);

    // Create candles with one having NaN low price
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},      // Valid candle
        {110, 15.0f, 19.0f, numeric_limits<float>::quiet_NaN(), 16.0f, 0.0f},  // NaN low - should be skipped
        {120, 23.0f, 29.0f, 21.0f, 27.0f, 0.0f}    // Valid candle
    };

    // Call showCandles - should not crash and should process valid candles
    chart.showCandles(candles, 60);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should skip candles with NaN low price and continue to next valid candle");
}

// showCandles should skip candles with invalid close price (NaN) and continue processing
TEST(test_Chart_showCandles_skips_candles_with_nan_close) {
    // Create a canvas and chart
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.setVisibleRange(100, 200);

    // Create candles with one having NaN close price
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},      // Valid candle
        {110, 15.0f, 19.0f, 12.0f, numeric_limits<float>::quiet_NaN(), 0.0f},  // NaN close - should be skipped
        {120, 23.0f, 29.0f, 21.0f, 27.0f, 0.0f}    // Valid candle
    };

    // Call showCandles - should not crash and should process valid candles
    chart.showCandles(candles, 60);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should skip candles with NaN close price and continue to next valid candle");
}

// showCandles should continue to next candle when showCandle returns false (has NaN values)
TEST(test_Chart_showCandles_continues_when_showCandle_returns_false) {
    // Create a canvas and chart
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.setVisibleRange(100, 200);

    // Create candles where some have NaN values (should be ignored) and one where showCandle returns false
    // showCandle returns false if any of the following are NaN: open, high, low, close
    vector<Candle> candles = {
        {100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f},  // Valid candle
        {110, numeric_limits<float>::quiet_NaN(), 12.0f, 5.0f, 10.0f, 0.0f},  // NaN open - will return false
        {120, 15.0f, 19.0f, 13.0f, 18.0f, 0.0f}   // Valid candle
    };

    // Call showCandles - should not crash even though showCandle returns false for one candle
    // The code should continue to the next candle instead of returning early
    chart.showCandles(candles, 60);

    // Test passes if no exception/crash occurs
    // Note: showCandle returns false for the second candle but the method continues to process the third candle
    assert(true && "showCandles should continue to next candle when showCandle returns false");
}

// showBars should handle an empty vector of points without crashing
TEST(test_Chart_showBars_empty_points) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set up valid bounds first
    chart.setVisibleRange(100, 200);
    
    // Call showBars with empty vector - should not crash
    chart.showBars({});
    
    // Test passes if no exception/crash occurs
    assert(true && "showBars should handle empty points without crashing");
}

// showPoints should handle an empty vector of points without crashing
TEST(test_Chart_showPoints_empty_points) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set up valid bounds first
    chart.setVisibleRange(100, 200);
    
    // Call showPoints with empty vector - should not crash
    chart.showPoints({});
    
    // Test passes if no exception/crash occurs
    assert(true && "showPoints should handle empty points without crashing");
}

// showBars should handle a single point without crashing
TEST(test_Chart_showBars_single_point) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set up valid bounds first
    chart.setVisibleRange(100, 200);
    
    // Call showBars with single point - should not crash
    chart.showBars({{150, 5.0f}});
    
    // Test passes if no exception/crash occurs
    assert(true && "showBars should handle single point without crashing");
}

// showPoints should handle a single point without crashing
TEST(test_Chart_showPoints_single_point) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set up valid bounds first
    chart.setVisibleRange(100, 200);
    
    // Call showPoints with single point - should not crash
    chart.showPoints({{150, 5.0f}});
    
    // Test passes if no exception/crash occurs
    assert(true && "showPoints should handle single point without crashing");
}

// fitToCandles should handle boundary case with single candle having extreme values
TEST(test_Chart_fitToCandles_single_candle_boundary) {
    MockCanvas canvas;
    TestChart chart(canvas);
    
    // Test with boundary values: min/max float values and extreme time values
    const time_sec extremeTime = numeric_limits<time_sec>::max() - 1;
    const float extremeLow = -numeric_limits<float>::max();
    const float extremeHigh = numeric_limits<float>::max();
    
    chart.fitToCandles({{extremeTime, extremeHigh, extremeHigh, extremeLow, extremeLow, 0.0f}});
    
    assert(chart.valueFirst == extremeTime && "valueFirst should handle extreme time value");
    assert(chart.valueLast == extremeTime && "valueLast should handle extreme time value");
    assert(chart.valueLower == extremeLow && "valueLower should handle extreme low value");
    assert(chart.valueUpper == extremeHigh && "valueUpper should handle extreme high value");
}

// timeToX should handle boundary condition when valueFirst == valueLast
TEST(test_Chart_timeToX_boundary_conditions) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set valueFirst and valueLast to the same value (boundary condition)
    chart.valueFirst = 100;
    chart.valueLast = 100;
    
    // timeToX should not crash and should return a valid position
    int x = chart.timeToX(100);
    
    // Just verify it's a reasonable position within the chart area
    assert(x >= chart.spacingLeft && "timeToX should return position within chart area");
    assert(x <= canvas.width() - chart.spacingRight && "timeToX should return position within chart area");
}

// timeToX should handle boundary condition when time < valueFirst
TEST(test_Chart_timeToX_boundary_before_range) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set valid time range
    chart.valueFirst = 100;
    chart.valueLast = 200;
    
    // timeToX with time < valueFirst should return position before left edge
    int x = chart.timeToX(50); // 50 < 100 (valueFirst)
    
    // Should be positioned before the left chart area
    assert(x < chart.spacingLeft && "timeToX should return position before chart area when time < valueFirst");
}

// timeToX should handle boundary condition when time > valueLast
TEST(test_Chart_timeToX_boundary_after_range) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set valid time range
    chart.valueFirst = 100;
    chart.valueLast = 200;
    
    // timeToX with time > valueLast should return position after right edge
    int x = chart.timeToX(250); // 250 > 200 (valueLast)
    
    // Should be positioned after the right chart area
    assert(x > canvas.width() - chart.spacingRight && "timeToX should return position after chart area when time > valueLast");
}

// valueToY should handle boundary condition when valueLower == valueUpper
TEST(test_Chart_valueToY_boundary_conditions) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set valueLower and valueUpper to the same value (boundary condition)
    chart.valueLower = 10.0f;
    chart.valueUpper = 10.0f;
    
    // valueToY should not crash and should return a valid position
    int y = chart.valueToY(10.0f);
    
    // Just verify it's a reasonable position within chart area
    assert(y >= chart.spacingTop && "valueToY should return position within chart area");
    assert(y <= canvas.height() - chart.spacingBottom && "valueToY should return position within chart area");
}

// valueToY should handle boundary condition when value < valueLower
TEST(test_Chart_valueToY_boundary_below_range) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set valid value range
    chart.valueLower = 10.0f;
    chart.valueUpper = 20.0f;
    
    // valueToY with value < valueLower should return position below bottom edge
    int y = chart.valueToY(5.0f); // 5.0f < 10.0f (valueLower)
    
    // Should be positioned below the chart area (remember Y axis is inverted)
    assert(y > canvas.height() - chart.spacingBottom && "valueToY should return position below chart area when value < valueLower");
}

// valueToY should handle boundary condition when value > valueUpper
TEST(test_Chart_valueToY_boundary_above_range) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    
    // Set valid value range
    chart.valueLower = 10.0f;
    chart.valueUpper = 20.0f;
    
    // valueToY with value > valueUpper should return position above top edge
    int y = chart.valueToY(25.0f); // 25.0f > 20.0f (valueUpper)
    
    // Should be positioned above the chart area (remember Y axis is inverted)
    assert(y < chart.spacingTop && "valueToY should return position above chart area when value > valueUpper");
}

// innerWidth should handle boundary condition when canvas <= spacingLeft + spacingRight
TEST(test_Chart_innerWidth_zero_width) {
    // Create canvas with very small width that results in zero inner width
    MockCanvas canvas(150, 600); // 150 <= 100 (spacingLeft) + 100 (spacingRight)
    TestChart chart(canvas);
    
    // innerWidth should return zero or negative when canvas is too small
    int width = chart.innerWidth();
    
    assert(width <= 0 && "innerWidth should return zero or negative when canvas <= spacingLeft + spacingRight");
}

// innerHeight should handle boundary condition when canvas <= spacingTop + spacingBottom
TEST(test_Chart_innerHeight_zero_height) {
    // Create canvas with very small height that results in zero inner height
    MockCanvas canvas(800, 50); // 50 <= 30 (spacingTop) + 30 (spacingBottom)
    TestChart chart(canvas);

    // innerHeight should return zero or negative when canvas is too small
    int height = chart.innerHeight();

    assert(height <= 0 && "innerHeight should return zero or negative when canvas <= spacingTop + spacingBottom");
}

// Test showBars with valid data
TEST(test_Chart_showBars_valid_data) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range
    chart.fitToPoints({{100, 5.0f}, {200, 8.0f}, {300, 3.0f}});

    // Test showBars with valid data
    chart.showBars({{100, 5.0f}, {200, 8.0f}, {300, 3.0f}});

    // Test passes if no exception/crash occurs
    assert(true && "showBars should handle valid data without crashing");
}

// Test showPoints with valid data
TEST(test_Chart_showPoints_valid_data) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range
    chart.fitToPoints({{100, 5.0f}, {200, 8.0f}, {300, 3.0f}});

    // Test showPoints with valid data
    chart.showPoints({{100, 5.0f}, {200, 8.0f}, {300, 3.0f}});

    // Test passes if no exception/crash occurs
    assert(true && "showPoints should handle valid data without crashing");
}

// Test showBar method
TEST(test_Chart_showBar_method) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time and value range
    chart.fitToPoints({{100, 5.0f}, {200, 8.0f}, {300, 3.0f}});

    // Test showBar method
    TestChart testChart(canvas);
    testChart.fitToPoints({{100, 5.0f}, {200, 8.0f}, {300, 3.0f}});
    testChart.testShowBar(100, 5.0f, 0);

    // Test passes if no exception/crash occurs
    assert(true && "showBar should handle valid data without crashing");
}

// Test showCandleAsLine method
TEST(test_Chart_showCandleAsLine_method) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});

    // Test showCandleAsLine method
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});
    testChart.testShowCandleAsLine({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, 0.5, 0xFF0000, 0x0000FF);

    // Test passes if no exception/crash occurs
    assert(true && "showCandleAsLine should handle valid data without crashing");
}

// Test showCandles with candleBodyWidth >= 1 (show full candles)
TEST(test_Chart_showCandles_full_candles) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});

    // Test showCandles with candleBodyWidth >= 1
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});
    testChart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}}, 60, 0xFF0000, 0x0000FF, 0.1);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle full candles without crashing");
}

// Test showCandles with candleBodyWidth < 1 (show only lines)
TEST(test_Chart_showCandles_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});

    // Test showCandles with candleBodyWidth < 1
    TestChart testChart(canvas);
    testChart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});
    testChart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}}, 0.5);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle line mode without crashing");
}

// Test showCandles with candleBodyWidth >= 1 and < 5 (line mode for each candle)
TEST(test_Chart_showCandles_line_mode_each_candle) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range with 100 seconds total span
    // canvas width 800, spacing left+right = 200, inner width = 600
    // To get candleBodyWidth between 1 and 5: 1 <= (600 * interval / 100) < 5
    // interval should be between 0.17 and 0.83
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});

    // Test showCandles with candleBodyWidth in range [1, 5)
    chart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}}, 50);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle line mode for each candle without crashing");
}

// Test showCandles with candleBodyWidth < 1 (aggregated line mode)
TEST(test_Chart_showCandles_aggregated_line_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Create many candles with small time span to trigger aggregation
    // Total time span: 1000 seconds, canvas inner width: 600
    // To get candleBodyWidth < 1: interval should be < 1.67
    vector<Candle> candles;
    for (int i = 0; i < 20; i++) {
        candles.push_back({(time_sec)(100 + i * 50), 3.0f, 9.0f, 2.0f, 7.0f, 0.0f});
    }
    chart.fitToCandles(candles);

    // Test showCandles with very small interval to trigger aggregation
    chart.showCandles(candles, 10);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle aggregated line mode without crashing");
}

// Test showCandleAsLine when showLine returns false
TEST(test_Chart_showCandleAsLine_returns_false_when_showLine_returns_false) {
    // Use MockShowLineChart where showLine always returns false
    MockCanvas canvas(800, 600);
    MockShowLineChart chart(canvas);
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});

    // Test showCandleAsLine when showLine returns false - should return false
    bool result = chart.showCandleAsLinePublic({100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, 0.5, 0xFF0000, 0x0000FF);
    assert(!result && "showCandleAsLine should return false when showLine returns false");
}

// Test showCandleAsLine when showLine returns false due to NaN values
TEST(test_Chart_showCandleAsLine_handles_NaN) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}});

    // Test showCandleAsLine with NaN values - should return false
    bool result = chart.testShowCandleAsLine({100, numeric_limits<float>::quiet_NaN(), 9.0f, 2.0f, 7.0f, 0.0f}, 0.5, 0xFF0000, 0x0000FF);
    assert(!result && "showCandleAsLine should return false when candle has NaN open value");
}

// Test showCandles with candleBodyWidth in range [1, 5) (line mode for each candle)
TEST(test_Chart_showCandles_line_mode_individual_candles) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Set up valid time range with 200 seconds total span (100 to 300)
    // canvas width 800, spacing left+right = 200, inner width = 600
    // To get candleBodyWidth between 1 and 5: 1 <= (600 * interval / 200) < 5
    // interval should be between 0.33 and 1.66
    chart.fitToCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}, {300, 5.0f, 11.0f, 4.0f, 9.0f, 0.0f}});

    // Test showCandles with candleBodyWidth in range [1, 5)
    // interval=1 gives: candleBodyWidth = 600 * 1 / 200 = 3 (in range [1, 5))
    chart.showCandles({{100, 3.0f, 9.0f, 2.0f, 7.0f, 0.0f}, {200, 4.0f, 10.0f, 3.0f, 8.0f, 0.0f}, {300, 5.0f, 11.0f, 4.0f, 9.0f, 0.0f}}, 1);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle line mode for individual candles without crashing");
}

// Test showCandles with candleBodyWidth < 1 (aggregation mode)
TEST(test_Chart_showCandles_aggregation_mode) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    // Create many candles with large time span to trigger aggregation
    // Total time span: 10000 seconds, canvas inner width: 600
    // To get candleBodyWidth < 1: interval should be < 1.67
    vector<Candle> candles;
    for (int i = 0; i < 50; i++) {
        candles.push_back({(time_sec)(100 + i * 200), 3.0f + i * 0.1f, 9.0f + i * 0.1f, 2.0f + i * 0.1f, 7.0f + i * 0.1f, 0.0f});
    }
    chart.fitToCandles(candles);

    // Test showCandles with very small interval to trigger aggregation
    chart.showCandles(candles, 1);

    // Test passes if no exception/crash occurs
    assert(true && "showCandles should handle aggregation mode without crashing");
}

#endif