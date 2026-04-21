#pragma once
#include "argument_processor.hpp"
#include "oss_clock.hpp"

namespace OSS {
    class OSS {
    private:
        pid_t pid_ = 0;
        bool needs_help_ = false;
        ArgumentProcessor *argument_processor_;
        OSSClock *oss_clock_;
    public:
        explicit OSS(int argc, char ** argv);
        int run();
    };
}