#pragma once
#include "argument_processor.hpp"
#include "oss_clock.hpp"
#include "oss_output.hpp"
#include "scheduler.hpp"
#include "../msg/msg_manager.hpp"

namespace OSS {
    class OSS {
    private:
        pid_t pid_ = 0;
        bool needs_help_ = false;
        OssOutput *oss_output_;
        ArgumentProcessor *argument_processor_;
        OSSClock *oss_clock_;
        ResourceManager *resource_manager_;
        Scheduler *scheduler_;
        MsgManager *msg_manager_;
    public:
        explicit OSS(int argc, char ** argv);
        int run();
    };
}