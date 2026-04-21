#include <fstream>
#include <sstream>
#include "../color/ui.hpp"
#include "argument_error.hpp"
#include "../error/error.hpp"

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



        void logProcessRequest();
        void logGrantRequest();
        void logBlockProcess();
        void logResourceRelease();
        void logDeadlock();
        void logResourceTable();
    };
}