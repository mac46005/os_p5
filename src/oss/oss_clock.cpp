#include "../../include/oss/oss_clock.hpp"

OSS::OSSClock::OSSClock(
    std::string key,
    float child_time_limit,
    float child_launch_limit,
    int time_quantum_sec,
    int time_quantum_nano): 
    time_quantum_(Time{time_quantum_sec, time_quantum_nano})
{
    clock_ = new Clock("OSS", key);
    child_time_limit_ = Clock::floatToTime(child_launch_limit);
    child_launch_time_limit_ = Clock::floatToTime(child_launch_limit);
}


void OSS::OSSClock::updateClockByQuantum() {
    Time *global_time = clock_->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, time_quantum_);
}


// DO I NEED THIS?
void OSS::OSSClock::updateOssTimeBy(Time time) {

}
///

Time OSS::OSSClock::getChildTimeLimit() {
    return child_time_limit_;
}

Time OSS::OSSClock::getCurrentTime() {
    Time current_time = *clock_->getCurrentTime();
    return current_time;
}