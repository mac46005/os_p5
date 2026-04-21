#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "argument_error.hpp"
#include "../error/error.hpp"
#include "../color/ui.hpp"
#include "oss_clock.hpp"


namespace OSS {
    class OssOutput {
    private:
        std::string file_name_;
        std::ofstream log_file_;
        
        void appendOption(Color::ColorBuilder &cb, std::string option, std::string arg);
        void appendOptionInfo(Color::ColorBuilder &cb, std::string option, std::string arg, std::string description);
        
        
    public:
        explicit OssOutput(std::string file_name);

        void printHelpMessage();
        void printArgumentErrorMessage(ArgumentError &argument_error);
        void printOssErrorMessage(Error &error);
        void printClock(OSSClock *clock);

        void openLogFile();
        void closeLogFile();
        void logProcessRequest();
        void logGrantRequest();
        void logBlockProcess();
        void logResourceRelease();
        void logDeadlock();
        void logResourceTable();


        void cleanUp();
    };
}