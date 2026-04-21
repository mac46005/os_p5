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
int UserProcess::run() {
    try {
        Color::printInfo("UserProcess", "running...");
        while(true) {
            msg_manager_->recieveMessage(
                [this](MsgBuffer msg) {
                    Color::printInfo("UserProcess", "Recieved message from OSS");
                },
                0
            );

            if (clock_checker_->isTimeReached()) {
                // send message to oss for terminating via time limit
                Color::printInfo("UserProcess", "Time reached. Terminating...");
                msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);
                break;
            }

            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::REQUEST, -1, 0);
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