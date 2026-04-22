#include "clock_checker.hpp"
#include "argument_processor.hpp"
#include "unistd.h"
#include "../color/ui.hpp"
#include "../msg/msg_manager.hpp"

class UserProcess {
private:
    pid_t pid_;
    pid_t ppid_;
    int held_resources_[10]{};
    ArgumentProcessor *argument_processor_;
    ClockChecker *clock_checker_;
    MsgManager *msg_manager_;
public:
    explicit UserProcess(int argc, char **argv);
    bool shouldRequest();
    bool hasAnyHeldResources();
    int chooseRequestResource();
    int chooseReleaseResource();
    int run();
    void cleanUp();
};