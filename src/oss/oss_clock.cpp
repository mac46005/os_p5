#include "../../include/oss/oss_clock.hpp"

OSS::OSSClock::OSSClock(
    std::string key,
    float child_time_limit,
    float child_launch_limit,
    int time_quantum_sec,
    int time_quantum_nano): 
    time_quantum_(Time{time_quantum_sec, time_quantum_nano})
{
        
}


void OSS::OSSClock::updateClockByQuantum() {
    Time *global_time = clock->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, time_quantum_);
}
