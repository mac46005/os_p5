#include "../../include/clock/clock.hpp"


Clock::Clock(string parent, string key): key_(key), parent_(parent) {
    string name_of_process = "Clock::Clock()";
    shm_key_ = ftok(key.c_str(), 1);
    if (shm_key_ == -1) {
        throw Error(parent_, name_of_process, "Failed to ftok()", strerror(errno));
    }

    shm_id_ = shmget(shm_key_, BUFF_SZ, IPC_CREAT | 0700);
    if (shm_id_ == -1) {
        throw Error(parent_, name_of_process, "Failed to shmget()", strerror(errno));
    }

    sim_clock_ = (Time *)shmat(shm_id_, 0, 0);
    if (sim_clock_ == (Time *)-1) {
        throw Error(parent_, name_of_process, "Failed to shmat()", strerror(errno));
    }

    sim_clock_->sec = 0;
    sim_clock_->nano = 0;


}

Time *Clock::getCurrentTime() {
    return sim_clock_;
}

void Clock::detach() {
    shmdt(sim_clock_);
}

void Clock::rmid() {
    shmctl(shm_id_, IPC_RMID, NULL);
}