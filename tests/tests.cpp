#include "../../misc/TEST.hpp"
#include "../../misc/ConsoleLogger.hpp"

#ifdef TEST
#include "test_Chart.hpp"
#include "test_ChartGroup.hpp"
#include "test_TimePointSeries.hpp"
#include "test_Fl_ChartBox.hpp"
#endif // TEST

int main() {
    createLogger<ConsoleLogger>();
    tester.run();
}
