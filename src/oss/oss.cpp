#include "../../include/oss/oss.hpp"

OSS::OSS::OSS(int argc, char **argv) {
    try {
        pid_ = getpid();
        argument_processor_ = new ArgumentProcessor(argc, argv);
        argument_processor_->processOptions(
            [this](Options options) {
                if (options.needsHelp == true) {
                    needs_help_ = options.needsHelp;
                } else {
                    oss_output_ = new OssOutput(options.logFile);

                    oss_clock_ = new OSSClock(
                        "./src/oss/oss.cpp",
                        options.childTimeLimit,
                        options.launchInterval,
                        0,
                        1000000
                    );

                    resource_manager_ = new ResourceManager();

                    scheduler_ = new Scheduler(
                        options.maxProc,
                        options.maxSimul,
                        this->oss_clock_,
                        this->resource_manager_,
                        this->oss_output_
                    );
                }
            }
        );
    } catch (ArgumentError &e) {

    } catch (Error &e) {

    } catch (std::exception &e) {

    }
}


int OSS::OSS::run() {
    if (needs_help_) {
        oss_output_->printHelpMessage();
    } else {
        try {
            oss_output_->openLogFile();
            
            while (
                (scheduler_->stillHaveChildrenToLaunch() || scheduler_->stillHaveChildrenInSystem())
            ) {
                scheduler_->launchChildrenIfAble();

                
            }
        } catch (Error &e) {

        } catch (std::exception &e) {

        }
    }
}