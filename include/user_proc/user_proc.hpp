#include "clock_checker.hpp"
#include "argument_processor.hpp"
#include "unistd.h"
#include "../color/ui.hpp"

class UserProcess {
private:
    ArgumentProcessor *argument_processor_;
    ClockChecker *clock_checker_;
public:
    explicit UserProcess(int argc, char **argv);
    int run();
    void cleanUp();
};