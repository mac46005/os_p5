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
                    oss_clock_ = new OSSClock("./src/oss/oss.cpp",options.childTimeLimit,options.launchInterval,0,1000000);
                }
            }
        );
    } catch (ArgumentError &e) {

    } catch (Error &e) {

    } catch (std::exception &e) {
        
    }
}