#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "argument_error.hpp"
#include "../error/error.hpp"
#include "../color/ui.hpp"
#include "pcb_queue.hpp"
#include "oss_clock.hpp"
#include "resource_manager.hpp"


namespace OSS {
    class OssOutput {
    private:
        std::string file_name_;
        std::ofstream log_file_;
        int file_line_count_ = 0;
        const int MAX_LINES = 10000;
        
        void appendOption(Color::ColorBuilder &cb, std::string option, std::string arg);
        void appendOptionInfo(Color::ColorBuilder &cb, std::string option, std::string arg, std::string description);
        void writeLine(const std::string &line);
        
    public:
        explicit OssOutput(std::string file_name);

        void printHelpMessage();
        void printArgumentErrorMessage(ArgumentError &argument_error);
        void printOssErrorMessage(Error &error);
        void printClock(OSSClock *clock);

        void openLogFile();
        void closeLogFile();
        void logProcessLaunch(pid_t pid, OSSClock *clock);
        void logProcessRequest(pid_t pid, int resource, OSSClock *clock);
        void logGrantRequest(pid_t pid, int resource, OSSClock *clock);
        void logBlockProcess(pid_t pid, int resource, OSSClock *clock);
        void logResourceRelease(pid_t pid, int resource, OSSClock *clock);
        void logDeadlock(const std::vector<PCB> &blocked, OSSClock *clock);
        void logResourceTable(const std::vector<PCB> &blocked_list, PCBQueue *ready_queue, ResourceManager *resource_manager);
        void logTerminateProcess(pid_t pid, OSSClock *clock);
        void logUnblockProcess(pid_t pid, int resource, OSSClock *clock);
        void logSystemTables(
            const std::vector<PCB> &blocked_list,
            PCBQueue *ready_queue,
            const PCB &current_process_running,
            ResourceManager *resource_manager,
            OSSClock *clock
        );
        void cleanUp();
    };
}