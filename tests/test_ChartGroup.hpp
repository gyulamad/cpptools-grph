#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../ChartGroup.hpp"
#include "MockCanvas.hpp"
#include "TestChart.hpp"
#include <vector>

using namespace std;

// ===== CHARTGROUP TESTS =====

// Test ChartGroup addChart and removeChart
TEST(test_ChartGroup_add_and_remove_chart) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart2.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart1.resetView();
    chart2.resetView();

    ChartGroup group;

    // Add charts
    group.addChart(chart1);
    group.addChart(chart2);

    // Zoom via group
    group.zoomAt(1.5, 400);

    // Both charts should be zoomed
    assert(chart1.viewLast - chart1.viewFirst < 1000 && "ChartGroup zoom should affect chart1");
    assert(chart2.viewLast - chart2.viewFirst < 1000 && "ChartGroup zoom should affect chart2");

    // Remove chart2
    group.removeChart(chart2);

    // Scroll via group - only chart1 should be affected
    group.scrollBy(100);

    // chart1 should have scrolled
    assert(chart1.viewFirst > 0 && "ChartGroup scroll should affect chart1 after chart2 removed");
}

// Test ChartGroup zoomAt with zoom out
TEST(test_ChartGroup_zoomOut) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart2.fitToPoints({{0, 0.0f}, {100, 10.0f}});  // Smaller range
    chart1.resetView();
    chart2.resetView();

    ChartGroup group;
    group.addChart(chart1);
    group.addChart(chart2);

    // chart1 is zoomed in to subset of data
    chart1.viewFirst = 200;
    chart1.viewLast = 800;

    // Zoom out via group - should work
    group.zoomAt(0.5, 400);

    // Verify zoom was applied
    assert(chart1.viewFirst < 200 || chart1.viewLast > 800 && "ChartGroup zoomAt should zoom out when factor < 1");
}

// Test ChartGroup zoomAt with factor < 1 when at full extent
TEST(test_ChartGroup_zoomOut_at_full_extent) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    chart.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart.resetView();

    ChartGroup group;
    group.addChart(chart);

    // Try to zoom out when view equals data - should clamp to data boundaries
    group.zoomAt(0.5, 400);

    // View should be clamped to data boundaries
    assert(chart.viewFirst == 0 && "ChartGroup zoomAt(0.5) should clamp to data boundaries");
    assert(chart.viewLast == 1000 && "ChartGroup zoomAt(0.5) should clamp to data boundaries");
}

// Test ChartGroup scrollBy affects all charts
TEST(test_ChartGroup_scrollBy_affects_all_charts) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    chart1.fitToPoints({{0, 0.0f}, {2000, 10.0f}});
    chart2.fitToPoints({{0, 0.0f}, {2000, 10.0f}});
    chart1.resetView();
    chart2.resetView();

    ChartGroup group;
    group.addChart(chart1);
    group.addChart(chart2);

    time_sec initial1 = chart1.viewFirst;
    time_sec initial2 = chart2.viewFirst;

    group.scrollBy(100);

    assert(chart1.viewFirst > initial1 && "ChartGroup scrollBy should shift chart1 forward");
    assert(chart2.viewFirst > initial2 && "ChartGroup scrollBy should shift chart2 forward");
}

// Test ChartGroup X-axis synchronization with different data ranges
TEST(test_ChartGroup_syncXAxis_different_ranges) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    // Chart1 has data from 0 to 1000
    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    // Chart2 has data from 500 to 1500 (overlapping but different range)
    chart2.fitToPoints({{500, 5.0f}, {1500, 15.0f}});
    chart1.resetView();
    chart2.resetView();

    // Create group with X-axis synchronization enabled (default)
    ChartGroup group(true);
    group.addChart(chart1);
    group.addChart(chart2);
    
    // Synchronize X-axis
    group.synchronizeXAxis();

    // Both charts should now have the same shared data bounds (0 to 1500)
    assert(chart1.valueFirst == 0 && "Chart1 should have shared valueFirst after sync");
    assert(chart1.valueLast == 1500 && "Chart1 should have shared valueLast after sync");
    assert(chart2.valueFirst == 0 && "Chart2 should have shared valueFirst after sync");
    assert(chart2.valueLast == 1500 && "Chart2 should have shared valueLast after sync");
    
    // Both charts should show the full shared range
    assert(chart1.viewFirst == 0 && "Chart1 viewFirst should be at shared start");
    assert(chart1.viewLast == 1500 && "Chart1 viewLast should be at shared end");
    assert(chart2.viewFirst == 0 && "Chart2 viewFirst should be at shared start");
    assert(chart2.viewLast == 1500 && "Chart2 viewLast should be at shared end");
}

// Test ChartGroup zoom with syncXAxis maintains shared bounds
TEST(test_ChartGroup_zoom_syncXAxis_maintains_shared_bounds) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    // Different data ranges
    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart2.fitToPoints({{500, 5.0f}, {1500, 15.0f}});
    chart1.resetView();
    chart2.resetView();

    ChartGroup group(true); // syncXAxis enabled
    group.addChart(chart1);
    group.addChart(chart2);
    
    // Synchronize first
    group.synchronizeXAxis();
    
    // Zoom in
    group.zoomAt(2.0, 400);

    // Both charts should have the same shared bounds (0 to 1500)
    assert(chart1.valueFirst == 0 && "Chart1 should maintain shared valueFirst after zoom");
    assert(chart1.valueLast == 1500 && "Chart1 should maintain shared valueLast after zoom");
    assert(chart2.valueFirst == 0 && "Chart2 should maintain shared valueFirst after zoom");
    assert(chart2.valueLast == 1500 && "Chart2 should maintain shared valueLast after zoom");
    
    // Both charts should have the same zoomed view
    assert(chart1.viewFirst == chart2.viewFirst && "Charts should have same viewFirst after zoom");
    assert(chart1.viewLast == chart2.viewLast && "Charts should have same viewLast after zoom");
}

// Test ChartGroup scroll with syncXAxis maintains shared bounds
TEST(test_ChartGroup_scroll_syncXAxis_maintains_shared_bounds) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    // Different data ranges
    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart2.fitToPoints({{500, 5.0f}, {1500, 15.0f}});
    chart1.resetView();
    chart2.resetView();

    ChartGroup group(true); // syncXAxis enabled
    group.addChart(chart1);
    group.addChart(chart2);
    
    // Synchronize first
    group.synchronizeXAxis();
    
    // Record initial view
    time_sec initialViewFirst = chart1.viewFirst;
    
    // Scroll
    group.scrollBy(100);

    // Both charts should have the same shared bounds
    assert(chart1.valueFirst == 0 && "Chart1 should maintain shared valueFirst after scroll");
    assert(chart1.valueLast == 1500 && "Chart1 should maintain shared valueLast after scroll");
    assert(chart2.valueFirst == 0 && "Chart2 should maintain shared valueFirst after scroll");
    assert(chart2.valueLast == 1500 && "Chart2 should maintain shared valueLast after scroll");
    
    // Both charts should have scrolled by the same amount
    assert(chart1.viewFirst == chart2.viewFirst && "Charts should have same viewFirst after scroll");
    assert(chart1.viewLast == chart2.viewLast && "Charts should have same viewLast after scroll");
    assert(chart1.viewFirst > initialViewFirst && "Charts should have scrolled forward");
}

// Test ChartGroup without syncXAxis (independent mode)
TEST(test_ChartGroup_no_syncXAxis_independent) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    // Different data ranges
    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart2.fitToPoints({{500, 5.0f}, {1500, 15.0f}});
    chart1.resetView();
    chart2.resetView();

    ChartGroup group(false); // syncXAxis disabled
    group.addChart(chart1);
    group.addChart(chart2);
    
    // Zoom in
    group.zoomAt(2.0, 400);

    // Charts should maintain their original data bounds
    assert(chart1.valueFirst == 0 && "Chart1 should keep original valueFirst without sync");
    assert(chart1.valueLast == 1000 && "Chart1 should keep original valueLast without sync");
    assert(chart2.valueFirst == 500 && "Chart2 should keep original valueFirst without sync");
    assert(chart2.valueLast == 1500 && "Chart2 should keep original valueLast without sync");
}

// Test ChartGroup setSyncXAxis toggle
TEST(test_ChartGroup_setSyncXAxis_toggle) {
    MockCanvas canvas1(800, 600);
    MockCanvas canvas2(800, 600);
    TestChart chart1(canvas1);
    TestChart chart2(canvas2);

    chart1.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart2.fitToPoints({{500, 5.0f}, {1500, 15.0f}});
    chart1.resetView();
    chart2.resetView();

    ChartGroup group(false); // Start with sync disabled
    group.addChart(chart1);
    group.addChart(chart2);
    
    // Enable sync
    group.setSyncXAxis(true);
    group.synchronizeXAxis();
    
    // Charts should now be synchronized
    assert(chart1.valueFirst == 0 && "Chart1 should have shared valueFirst after enabling sync");
    assert(chart1.valueLast == 1500 && "Chart1 should have shared valueLast after enabling sync");
    assert(chart2.valueFirst == 0 && "Chart2 should have shared valueFirst after enabling sync");
    assert(chart2.valueLast == 1500 && "Chart2 should have shared valueLast after enabling sync");
}


#endif
