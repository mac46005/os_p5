#include "../../include/user_proc/user_proc.hpp"

UserProcess::UserProcess(int argc, char **argv) {
    argument_processor_ = new ArgumentProcessor(argc, argv);

    argument_processor_->process(
        [this](Options options) {
            clock_checker_ = new ClockChecker(options.sec, options.nano);
        }
    );
}
int UserProcess::run() {
    try {
        Color::printInfo("UserProcess", "running...");
        while(true) {
            if (clock_checker_->isTimeReached()) {
                // send message to oss for terminating via time limit
                Color::printInfo("UserProcess", "Time reached. Terminating...");
                break;
            }
        }

        cleanUp();
    } catch (std::exception &e) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void UserProcess::cleanUp() {
    clock_checker_->cleanUp();
}