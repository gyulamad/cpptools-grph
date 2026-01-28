#pragma once

#include "../Fl_ChartBox.hpp"

class MockFl_ChartBox : public Fl_ChartBox {
public:
    using Fl_ChartBox::Fl_ChartBox;
    
    // Expose protected members for testing
    using Fl_ChartBox::chart;
    using Fl_ChartBox::candlesSerieses;
    using Fl_ChartBox::barsSerieses;
    using Fl_ChartBox::pointsSerieses;
    using Fl_ChartBox::group;
    using Fl_ChartBox::lastDragX;
    
    // Expose protected methods for testing
    using Fl_ChartBox::onMouseWheel;
    using Fl_ChartBox::onDrag;
};
