#include "../../include/oss/oss_clock.hpp"

OSS::OSSClock::OSSClock(
    std::string key,
    int child_time_limit_sec,
    int child_time_limit_nano,
    int child_launch_limit_sec,
    int child_launch_limit_nano,
    int time_quantum_sec,
    int time_quantum_nano): 
    child_time_limit_(Time{child_time_limit_sec, child_time_limit_nano}),
    child_launch_time_limit_(Time{child_launch_limit_sec, child_launch_limit_nano}),
    time_quantum_(Time{time_quantum_sec, time_quantum_nano})
{
        
}


void OSS::OSSClock::updateClockByQuantum() {
    Time *global_time = clock->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, time_quantum_);
}
