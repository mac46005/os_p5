#include "../../include/oss/scheduler.hpp"

OSS::Scheduler::Scheduler(
    int max_proc,
    int max_simul,
    OSSClock *oss_clock,
    ResourceManager *resource_manager,
    OssOutput *oss_output,
    MsgManager *msg_manager) : oss_clock_(oss_clock), resource_manager_(resource_manager), oss_output_(oss_output), msg_manager_(msg_manager)
{
    pcb_info_.max_process_count_ = max_proc;
    pcb_info_.max_simultaneous_count_ = max_simul;
    pcb_ready_queue_ = new PCBQueue();
}

OSS::PCB OSS::Scheduler::createPCB(pid_t pid)
{
    Time current_time = oss_clock_->getCurrentTime();
    OSS::PCB pcb{
        .pid = pid,
        .start_sec = current_time.sec,
        .start_nano = current_time.nano,
        .requested_resource = -1
    };

    for (auto &i : pcb.resource_allocated) {
        i = 0;
    }

    return pcb;
}

void OSS::Scheduler::forkProcess()
{
    Time current_time = oss_clock_->getCurrentTime();

    Time child_time_limit_ = oss_clock_->getChildTimeLimit();
    Clock::addTimeToPtrTime(&child_time_limit_, current_time);

    std::string child_runtime_sec_str = std::to_string(child_time_limit_.sec);
    std::string child_runtime_nano_str = std::to_string(child_time_limit_.nano);

    char *args[] = {
        const_cast<char *>("./user_proc"),
        const_cast<char *>(child_runtime_sec_str.c_str()),
        const_cast<char *>(child_runtime_nano_str.c_str()),
        nullptr};

    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
        perror("excvp failed");
        _exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        pcb_info_.pcb_count_++;
        pcb_info_.process_count_++;
        PCB pcb = createPCB(pid);
        pcb_ready_queue_->enqueue(pcb);

        // output to log
        oss_output_->logProcessLaunch(pid, oss_clock_);
    }
}

bool OSS::Scheduler::stillHaveChildrenToLaunch()
{
    return !pcb_info_.isProcCountReached();
}
bool OSS::Scheduler::stillHaveChildrenInSystem()
{
    return pcb_info_.hasChildrenInSystem() || current_process_running_.pid != -1;
}

void OSS::Scheduler::launchChildrenIfAble()
{
    if (pcb_info_.hasOpenPCBSlot() && oss_clock_->launchIntervalReached())
    {
        if (
            !pcb_info_.isSimulCountReached())
        {
            pcb_info_.simultaneous_count_++;
            forkProcess();
        }
        else if (
            !pcb_info_.isProcCountReached())
        {
            // ADD FLAG THAT LINEAR PROCESS RUN
            forkProcess();
        }
        oss_clock_->resetLaunchInterval();
    }
}












void OSS::Scheduler::requeueCurrentProcess() {
    if (current_process_running_.pid > 0) {
        pcb_ready_queue_->enqueue(current_process_running_);
        current_process_running_= PCB{.pid = -1};
    }
}

void OSS::Scheduler::blockCurrentProcess(int resource) {
    current_process_running_.requested_resource = resource;
    pcb_blocked_list.push_back(current_process_running_);
    current_process_running_ = PCB{.pid = -1};
}

void OSS::Scheduler::releaseCurrentProcessResources() {
    resource_manager_->releaseAll(current_process_running_);
}
void OSS::Scheduler::canUnblockBlockedProcess()
{
    std::vector<PCB> still_blocked;

        for (auto &pcb : pcb_blocked_list) {
            int resource = pcb.requested_resource;

            if (resource_manager_->grant(pcb, resource)) {
                // log unblock/grant here
                oss_output_->logUnblockProcess(pcb.pid, resource, oss_clock_);
                pcb_ready_queue_->enqueue(pcb);
            } else {
                still_blocked.push_back(pcb);
            }
        }

    pcb_blocked_list = still_blocked;
}

void OSS::Scheduler::terminateProcess()
{
    pcb_info_.pcb_count_--;

    releaseCurrentProcessResources();

    Time current_time = oss_clock_->getCurrentTime();

    current_process_running_.end_sec = current_time.sec;
    current_process_running_.end_nano = current_time.nano;
    // output to log
    oss_output_->logTerminateProcess(current_process_running_.pid, oss_clock_);
    completed_processes.push_back(current_process_running_);
    
    current_process_running_ = PCB{.pid = -1};

    canUnblockBlockedProcess();
}



void OSS::Scheduler::handleRequest(const MsgBuffer &msg) {
    int resource = msg.resource;
    oss_output_->logProcessRequest(current_process_running_.pid, resource, oss_clock_);
    if (resource_manager_->grant(current_process_running_, resource)) {
        // log output here
        oss_output_->logGrantRequest(current_process_running_.pid, resource, oss_clock_);
        requeueCurrentProcess();
    } else {
        oss_output_->logBlockProcess(current_process_running_.pid, resource, oss_clock_);
        blockCurrentProcess(resource);
    }
}

void OSS::Scheduler::handleRelease(const MsgBuffer &msg) {
    int resource = msg.resource;

    resource_manager_->releaseOne(current_process_running_, resource);

    // log output here
    oss_output_->logResourceRelease(current_process_running_.pid, resource, oss_clock_);
    requeueCurrentProcess();
    canUnblockBlockedProcess();
}

void OSS::Scheduler::handleTerminate() {
    terminateProcess();
}

void OSS::Scheduler::updateProcessInReadyQueue()
{
    if (current_process_running_.pid != -1) {
        return;
    }

    if (pcb_ready_queue_->isEmpty()) {
        return;
    }

    current_process_running_ = pcb_ready_queue_->dequeue();

    msg_manager_->sendMessage(
        current_process_running_.pid,
        getpid(),
        ProcessStatus::OSS_CONTROL,
        -1,
        0
    );

    msg_manager_->recieveMessage(
        [this](MsgBuffer msg) {
            switch (msg.status) {
                case ProcessStatus::REQUEST:
                    handleRequest(msg);
                    break;
                case ProcessStatus::RELEASE:
                    handleRelease(msg);
                    break;
                case ProcessStatus::TERMINATE:
                    handleTerminate();
                    break;
                default:
                    break;
            }
        },
        0
    );
}

void OSS::Scheduler::cleanUp()
{
    if (current_process_running_.pid > 0) {
        kill(current_process_running_.pid, SIGTERM);
    }

    for (auto &pcb : pcb_blocked_list) {
        if (pcb.pid > 0) {
            kill(pcb.pid, SIGTERM);
        }
    }

    pcb_ready_queue_->traverse(
        [](PCB *pcb) {
            if (pcb->pid > 0) {
                kill(pcb->pid, SIGTERM);
            }
        }
    );

    int status = 0;
    while (waitpid(-1, &status, 0) > 0){};
}