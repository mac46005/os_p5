#pragma once
#include "../clock/clock.hpp"

namespace OSS {
    class OSSClock {
    private:
        Clock *clock;


        Time time_quantum_{0,0};
        Time oss_work_time_{0,0};
        Time child_time_limit_{0,0};
        Time child_launch_time_limit_{0,0};
        Time current_child_launch_time_{0,0};




    public:
        explicit OSSClock(
            std::string key, 
            float child_time_limit_, 
            float child_launch_limit_, 
            int time_quantum_sec,
            int time_quantum_nano
        ) {}
        void updateClockByQuantum();
        void updateOssTimeBy(Time time);
        
    };
}