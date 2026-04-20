#include "../clock/clock.hpp"

namespace OSS {
    class OSSClock {
    private:
        Clock *clock;

        Time oss_work_time_{0,0};
        Time child_time_limit_{0,0};
        Time child_launch_time_limit_{0,0};
        Time current_child_launch_time_{0,0};




    public:
        explicit OSSClock(string key, int child_time_limit_sec, int child_time_limit_nano, int child_launch_limit_sec, int child_launch_limit_nano) {}
        void updateClockBy(Time time);
        void updateOssTimeBy(Time time);
        
    };
}