#include "../../include/clock/clock.hpp"


Clock::Clock(std::string parent, std::string key): key_(key), parent_(parent) {
    std::string name_of_process = "Clock::Clock()";
    shm_key_ = ftok(key_.c_str(), 1);
    if (shm_key_ == -1) {
        throw Error(parent_, name_of_process, "Failed to ftok()", strerror(errno));
    }

    shm_id_ = shmget(shm_key_, BUFF_SZ, IPC_CREAT | 0700);
    if (shm_id_ == -1) {
        throw Error(parent_, name_of_process, "Failed to shmget()", strerror(errno));
    }

    current_time_ = (Time *)shmat(shm_id_, 0, 0);
    if (current_time_ == (Time *)-1) {
        throw Error(parent_, name_of_process, "Failed to shmat()", strerror(errno));
    }

    current_time_->sec = 0;
    current_time_->nano = 0;


}

Time *Clock::getCurrentTime() {
    return current_time_;
}

void Clock::addTimeToPtrTime(Time *ptr_time, Time add_time) {
    ptr_time->sec = ptr_time->sec + add_time.sec;
    ptr_time->nano = ptr_time->nano + add_time.nano;

    if (ptr_time->nano >= Clock::SECOND_TO_NANO) {
        ptr_time->sec++;
        ptr_time->nano -= Clock::SECOND_TO_NANO;
    }
}

void Clock::subtractTimeToPtrTime(Time *ptr_time, Time subtract_time) {
    ptr_time->sec -= subtract_time.sec;
    ptr_time->nano -= subtract_time.nano;

    if (ptr_time->nano < 0) {
        ptr_time->sec--;
        ptr_time->nano += Clock::SECOND_TO_NANO;
    }
}


// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
Time Clock::floatToTime(float time_f) {
    Time time{0,0};
    int sec = static_cast<int>(time_f);
    float decimal = time_f - sec;
    decimal = decimal * 10;
    int nano = static_cast<int>(decimal);
    time.sec = sec;
    time.nano = nano;
    return time;
}
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
// WARNING WARNING WARNING WARNING WARNING WARNING
void Clock::detach() {
    shmdt(current_time_);
}

void Clock::rmid() {
    shmctl(shm_id_, IPC_RMID, NULL);
}


std::string Clock::toString() {
    std::string time_str = std::to_string(current_time_->sec) + ":" + std::to_string(current_time_->nano);
    return time_str;
}