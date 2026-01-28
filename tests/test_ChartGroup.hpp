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

// Test ChartGroup canZoomOut()
TEST(test_ChartGroup_canZoomOut) {
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

    // chart1 is zoomed in to subset of data, so canZoomOut should return true
    chart1.viewFirst = 200;
    chart1.viewLast = 800;

    assert(group.canZoomOut() && "ChartGroup canZoomOut should return true when any chart has data outside view");

    // Zoom out via group - should work
    group.zoomAt(0.5, 400);

    // Verify zoom was applied
    assert(chart1.viewFirst < 200 || chart1.viewLast > 800 && "ChartGroup zoomAt should zoom out when factor < 1");
}

// Test ChartGroup zoomAt with factor < 1 when cannot zoom out
TEST(test_ChartGroup_zoomOut_prevented) {
    MockCanvas canvas(800, 600);
    TestChart chart(canvas);

    chart.fitToPoints({{0, 0.0f}, {1000, 10.0f}});
    chart.resetView();

    ChartGroup group;
    group.addChart(chart);

    // Try to zoom out when view equals data - should be prevented
    group.zoomAt(0.5, 400);

    // View should not change since cannot zoom out further
    assert(chart.viewFirst == 0 && "ChartGroup zoomAt(0.5) should be prevented when cannot zoom out");
    assert(chart.viewLast == 1000 && "ChartGroup zoomAt(0.5) should be prevented when cannot zoom out");
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

// Test ChartGroup empty group operations
TEST(test_ChartGroup_empty_operations) {
    ChartGroup group;

    // These should not crash
    group.zoomAt(1.5, 400);
    group.scrollBy(100);
    assert(!group.canZoomOut() && "Empty group canZoomOut should return false");

    assert(true && "Empty ChartGroup operations should not crash");
}

#endif
