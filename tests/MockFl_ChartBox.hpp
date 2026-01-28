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
};
