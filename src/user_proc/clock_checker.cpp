#include "../../include/user_proc/clock_checker.hpp"

ClockChecker::ClockChecker(int max_sec, int max_nano) {
    time_limit_.sec = max_sec;
    time_limit_.nano = max_nano;
    clock_ = new Clock("User Process", "./src/oss/oss.cpp");
}

bool ClockChecker::isTimeReached() {
    Time current_time = *clock_->getCurrentTime();
    if (
        (current_time.sec >= time_limit_.sec)
        ||
        (current_time.sec == time_limit_.sec && current_time.nano >= time_limit_.nano)
    ) {
        return true;
    } else {
        return false;
    }
}

void ClockChecker::cleanUp() {
    clock_->detach();
}