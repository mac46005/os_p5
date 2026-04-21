#pragma once
#include "../clock/clock.hpp"

namespace OSS {
    class OSSClock {
    private:
        Clock *clock_;


        Time time_quantum_{0,0};
        Time oss_work_time_{0,0};
        Time child_time_limit_{0,0};
        Time child_launch_time_limit_{0,0};
        Time child_launch_time_{0,0};

        bool is_launch_interval_time_reached_ = false;
        void checkIfLaunchIntervalReached();


    public:
        explicit OSSClock(
            std::string key, 
            float child_time_limit, 
            float child_launch_limit, 
            int time_quantum_sec,
            int time_quantum_nano
        );

        void updateClockByQuantum();
        void updateOssTimeBy(Time time);
        Time getChildTimeLimit();
        Time getCurrentTime();
        
        void resetLaunchInterval();
        bool launchIntervalReached();
    };
}