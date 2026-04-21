#include "../../include/oss/scheduler.hpp"

OSS::Scheduler::Scheduler(
    int max_proc, 
    int max_simul, 
    OSSClock *oss_clock, 
    ResourceManager * resource_manager, 
    OssOutput *oss_output
): oss_clock_(oss_clock), resource_manager_(resource_manager), oss_output_(oss_output) {
    pcb_info_.max_process_count_ = max_proc;
    pcb_info_.max_simultaneous_count_ = max_simul;
}

OSS::PCB OSS::Scheduler::createPCB(pid_t pid) {
    Time current_time = oss_clock_->getCurrentTime();
    PCB pcb{
        .pid = pid,
        .start_sec = current_time.sec,
        .start_nano = current_time.nano
    };

    return pcb;
}

void OSS::Scheduler::forkProcess() {
    Time child_time_limit_ = oss_clock_->getChildTimeLimit();
    std::string child_runtime_sec_str = std::to_string(child_time_limit_.sec);
    std::string child_runtime_nano_str = std::to_string(child_time_limit_.nano);

    char *args[] = {
        const_cast<char *>("./user_proc"),
        const_cast<char *>(child_runtime_sec_str.c_str()),
        const_cast<char *>(child_runtime_nano_str.c_str())
    };

    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("excvp failed");
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        pcb_info_.pcb_count_++;
        pcb_info_.process_count_++;
        PCB pcb = createPCB(pid);
        pcb_ready_queue_->enqueue(pcb);

        // output to log
    }
}




void OSS::Scheduler::launchChildrenIfAble() {
    if (pcb_info_.hasOpenPCBSlot()) {
        if (
            !pcb_info_.isSimulCountReached()
        ) {
            pcb_info_.simultaneous_count_++;
            forkProcess();
        } else if (
            !pcb_info_.isProcCountReached()
        ) {
            // ADD FLAG THAT LINEAR PROCESS RUN
            forkProcess();
        }
    }

    // UPDATE OSS WORK TIME?
    
}