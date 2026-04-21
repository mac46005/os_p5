#include "../../include/oss/oss_clock.hpp"

OSS::OSSClock::OSSClock(
    std::string key,
    float child_time_limit,
    float child_launch_limit,
    int time_quantum_sec,
    int time_quantum_nano) : time_quantum_(Time{time_quantum_sec, time_quantum_nano})
{
    clock_ = new Clock("OSS", key);
    child_time_limit_ = Clock::floatToTime(child_time_limit);
    child_launch_time_limit_ = Clock::floatToTime(child_launch_limit);
}

void OSS::OSSClock::updateClockByQuantum()
{
    Time *global_time = clock_->getCurrentTime();
    Clock::addTimeToPtrTime(global_time, time_quantum_);
}

// DO I NEED THIS?
void OSS::OSSClock::updateOssTimeBy(Time time)
{
}
///

Time OSS::OSSClock::getChildTimeLimit()
{
    return child_time_limit_;
}

Time OSS::OSSClock::getCurrentTime()
{
    Time current_time = *clock_->getCurrentTime();
    return current_time;
}











void OSS::OSSClock::checkIfLaunchIntervalReached() {
    Time current_time_ = getCurrentTime();

    bool reached = ((current_time_.sec > child_launch_time_.sec) || (current_time_.sec == child_launch_time_.sec && current_time_.nano >= child_launch_time_.nano));
    is_launch_interval_time_reached_ = reached;
}
void OSS::OSSClock::resetLaunchInterval()
{
    Time *current_time = clock_->getCurrentTime();
    Time new_launch_time{0, 0};
    Clock::addTimeToPtrTime(&new_launch_time, *current_time);
    child_launch_time_ = new_launch_time;
    is_launch_interval_time_reached_ = false;
}
bool OSS::OSSClock::launchIntervalReached() {
    checkIfLaunchIntervalReached();
    return is_launch_interval_time_reached_;
}






void OSS::OSSClock::cleanUp() {
    clock_->detach();
    clock_->rmid();
}









