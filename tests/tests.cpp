#include "../../misc/TEST.hpp"
#include "../../misc/ConsoleLogger.hpp"
#include "../../misc/Arguments.hpp"
#include "../../misc/explode.hpp"

#ifdef TEST
#include "test_Chart.hpp"
#include "test_ChartGroup.hpp"
#include "test_TimePointSeries.hpp"
#include "test_Fl_ChartBox.hpp"
#endif // TEST

int main(int argc, char** argv) {
    createLogger<ConsoleLogger>();
    Arguments args(argc, argv); // TODO: this may can be in tester instead?
    args.addHelper("filter", "Filter tests by name - optinal, comma separated.");
    const vector<string> filter = trim(explode(",", args.getopt<string>("filter", "")));
    tester.run(filter);
}
