#include "../TEST.hpp"
#include "../ConsoleLogger.hpp"



int main() {
    createLogger<ConsoleLogger>();
    tester.run();
}
