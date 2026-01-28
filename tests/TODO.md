# TODO: Chart Tests for Full Coverage

## Missing Tests for Current Coverage

### Chart.hpp - Uncovered Methods
- [x] `test_Chart_showCandles_with_empty_vector`
- [x] `test_Chart_showBars_empty_points`
- [x] `test_Chart_showPoints_empty_points`
- [x] `test_Chart_showBars_single_point`
- [x] `test_Chart_showPoints_single_point`
- [x] `test_Chart_fitToCandles_single_candle_boundary` (boundary testing for fitToCandles)
- [x] `test_Chart_timeToX_boundary_conditions` (when valueFirst == valueLast)
- [x] `test_Chart_timeToX_boundary_before_range` (time < valueFirst)
- [x] `test_Chart_timeToX_boundary_after_range` (time > valueLast)
- [x] `test_Chart_valueToY_boundary_conditions` (when valueLower == valueUpper)
- [x] `test_Chart_valueToY_boundary_below_range` (value < valueLower)
- [x] `test_Chart_valueToY_boundary_above_range` (value > valueUpper)
- [x] `test_Chart_innerWidth_zero_width` (canvas <= spacingLeft + spacingRight)
- [x] `test_Chart_innerHeight_zero_height` (canvas <= spacingTop + spacingBottom)

### MockCanvas Enhancement
- [ ] Add configurable dimensions to MockCanvas for better real usage simulation

## New Tests for Zoom/Scroll Feature

### View Management Tests
- [ ] `test_Chart_setFullData_initializes_view_window`
- [ ] `test_Chart_getVisibleCandles_subset`
- [ ] `test_Chart_getVisibleCandles_all_when_fully_visible`
- [ ] `test_Chart_getVisibleCandles_empty_outside_range`

### Zoom Tests
- [ ] `test_Chart_zoomAt_zooms_in` (factor > 1)
- [ ] `test_Chart_zoomAt_zooms_out` (factor < 1)
- [ ] `test_Chart_zoomAt_centered_on_mouse` (zoom centered on X position)
- [ ] `test_Chart_zoomAt_boundary_limits` (min/max zoom limits)
- [ ] `test_Chart_zoomAt_single_candle_minimum` (can't zoom smaller than one candle)

### Scroll Tests
- [ ] `test_Chart_scrollBy_positive_delta` (right scroll)
- [ ] `test_Chart_scrollBy_negative_delta` (left scroll)
- [ ] `test_Chart_scrollBy_boundary_clamping` (stops at data boundaries)

### Coordinate Conversion Tests
- [ ] `test_Chart_pixelToTime_conversion`
- [ ] `test_Chart_timeToPixel_conversion`
- [ ] `test_Chart_coordinate_conversion_roundtrip`

### Integration Tests
- [ ] `test_Chart_zoom_scroll_integration`
- [ ] `test_Chart_edge_case_zoom_after_scroll`

## Fl_ChartBox Tests (New File Needed)

### Mouse Callback Tests
- [ ] `test_Fl_ChartBox_mouse_callback_setup`
- [ ] `test_Fl_ChartBox_scroll_callback_zoom`
- [ ] `test_Fl_ChartBox_drag_callback_scroll`

### Drawing Tests
- [ ] `test_Fl_ChartBox_draw_uses_visible_candles`
- [ ] `test_Fl_ChartBox_multiple_series_handling`

## MockCanvas Enhancement
- [ ] Add method call tracking for coverage verification
- [ ] Track drawing parameters for test validation

## Notes
- Follow naming convention: `test_<classname>_<method>_<case_descriptor>`
- Use descriptive assertion messages
- Test boundary conditions and edge cases
- Verify coordinate conversion accuracy
- Test mouse event integration
- Ensure no regressions in existing functionality