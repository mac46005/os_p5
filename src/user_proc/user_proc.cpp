#include "../../include/user_proc/user_proc.hpp"

UserProcess::UserProcess(int argc, char **argv) {
    pid_ = getpid();
    ppid_ = getppid();

    argument_processor_ = new ArgumentProcessor(argc, argv);

    argument_processor_->process(
        [this](Options options) {
            clock_checker_ = new ClockChecker(options.sec, options.nano);
        }
    );

    msg_manager_ = new MsgManager("msgq.txt", 0644, pid_);
}

bool UserProcess::shouldRequest() {
    return (rand() % 100) < 70;
}
bool UserProcess::hasAnyHeldResources() {
    for (int i = 0; i < 10; i++) {
        if (held_resources_[i] > 0) {
            return true;
        }
    }
    return false;
}

int UserProcess::chooseReleaseResource() {

    // may need to change this to see if resource is more than 5 
    return rand() % 10;
}
int UserProcess::chooseReleaseResource() {
    std::vector<int> owned;
    for (int i = 0; i < 10; i++) {
        if (held_resources_[i] > 0) {
            owned.push_back(i);
        }
    }

    if (owned.empty()) {
        return -1;
    }

    return owned[rand() % owned.size()];
}
int UserProcess::run() {
    try {
        Color::printInfo("UserProcess", "running...");
        while(true) {
            msg_manager_->recieveMessage(
                [this](MsgBuffer msg) {
                    if (msg.status == ProcessStatus::GRANTED && msg.resource >= 0 && msg.resource < 10) {
                        held_resources_[msg.resource]++;
                    }
                    // dont forget that parent can force kill you
                },
                0
            );

            if (clock_checker_->isTimeReached()) {
                // send message to oss for terminating via time limit
                Color::printInfo("UserProcess", "Time reached. Terminating...");
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);
                break;
            }

            if (shouldRequest()) {
                int resource = chooseRequestResource();
                if (resource >= 0) {
                    msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::REQUEST, resource, 0);
                }
            } else {
                int resource = chooseReleaseResource();
                if (resource >= 0) {
                    held_resources_[resource]--;
                    msg_manager_->sendMessage(ppid_,pid_, ProcessStatus::RELEASE, resource, 0);
                } else {
                    int resource_requested = chooseRequestResource();
                    if (resource_requested   >= 0) {
                        msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::RELEASE, resource_requested, 0);
                    }
                }
            }
        }
        Color::printInfo("UserProcess", "Terminating...");

        cleanUp();
    } catch (std::exception &e) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void UserProcess::cleanUp() {
    clock_checker_->cleanUp();
}