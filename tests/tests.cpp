#include "../../misc/TEST.hpp"
#include "../../misc/ConsoleLogger.hpp"

#ifdef TEST
#include "test_Chart.hpp"
#include "test_TimePointSeries.hpp"
#endif // TEST

int main() {
    createLogger<ConsoleLogger>();
    tester.run();
}
