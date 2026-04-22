#include "../../include/oss/oss_output.hpp"
OSS::OssOutput::OssOutput(std::string file_name) : file_name_(file_name)
{
    int fd = open(file_name_.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        throw std::runtime_error("OssOutput failed to open file " + file_name);
    }
    close(fd);
}
void OSS::OssOutput::appendOption(
    Color::ColorBuilder &cb,
    std::string option,
    std::string arg)
{
    Color::optionalContainerStart(cb);
    Color::space(cb);
    Color::option(cb, option);
    Color::space(cb);
    Color::optionArgument(cb, arg);
    Color::space(cb);
    Color::optionalContainerEnd(cb);
}

void OSS::OssOutput::appendOptionInfo(Color::ColorBuilder &cb, std::string option, std::string arg, std::string description)
{
    appendOption(cb, option, arg);
    Color::newLine(cb);
    Color::tab(cb);
    Color::defaultOutput(cb, description);
    Color::newLine(cb);
}

void OSS::OssOutput::printHelpMessage()
{
    Color::ColorBuilder cb;

    Color::title(cb, "Project 5: The Resource Manager");
    Color::newLine(cb);
    Color::label(cb, "USAGE");
    Color::space(cb);
    Color::title(cb, "oss");
    Color::space(cb);
    appendOption(cb, "n", "proc");
    Color::space(cb);
    appendOption(cb, "s", "simul");
    Color::space(cb);
    appendOption(cb, "t", "childTimeLimit");
    Color::space(cb);
    appendOption(cb, "i", "launchInterval");
    Color::space(cb);
    appendOption(cb, "f", "logfile");
    Color::newLine(cb);

    Color::label(cb, "PURPOSE");
    Color::defaultOutput(
        cb,
        "This program simulates an operating system that launches several worker processes.\n"
        "The simulated oss manages and schedules workers to work a certain time limit and also manage resources.\n"
        "All workers can decide how to utilize \"working\" time and resources.");
    Color::newLine(cb);

    Color::label(cb, "ARGUMENTS");
    Color::newLine(cb);
    appendOptionInfo(cb, "n", "proc", "Max processes to launch");
    appendOptionInfo(cb, "s", "simul", "Max simultaneous processes to launch");
    appendOptionInfo(cb, "t", "childTimeLimit", "Total bound time limit for children to execute for");
    appendOptionInfo(cb, "i", "launchInterval", "Total bound time launch interval");
    appendOptionInfo(cb, "f", "logfile", "The name of the log file to output oss operations");

    std::cout << cb.build();
}

void OSS::OssOutput::printArgumentErrorMessage(ArgumentError &argument_error)
{
    Color::ColorBuilder cb;
    Color::appendWarningMark(cb);
    Color::label(cb, "ARGUMENT");
    Color::space(cb);
    Color::optionalContainerStart(cb);
    Color::option(cb, std::string("-") + std::string(argument_error.getOption()));
    Color::space(cb);
    Color::optionArgument(cb, argument_error.getArg());
    Color::optionalContainerEnd(cb);
    Color::newLine(cb);
    Color::label(cb, "MESSAGE");
    Color::newLine(cb);
    Color::defaultOutput(cb, argument_error.getMessage());
    Color::appendWarningMark(cb);

    std::cout << cb.build();
}

void OSS::OssOutput::printOssErrorMessage(Error &error)
{
    std::string oss = "OSS";

    Color::ColorBuilder cb;
    Color::appendErrorMark(cb);
    Color::title(cb, oss);
    Color::newLine(cb);

    Color::label(cb, "SUBJECT");
    Color::title(cb, oss);
    Color::appendScopeOperator(cb);
    cb.appendForeground(Color::Colors::CYAN, error.getSubject());
    Color::newLine(cb);

    Color::label(cb, "FROM");
    Color::space(cb);
    Color::title(cb, oss);
    Color::appendScopeOperator(cb);
    cb.appendForeground(Color::Colors::CYAN, error.getSubject());
    Color::appendScopeOperator(cb);
    cb.appendForeground(Color::Colors::YELLOW, error.getFrom());
    Color::newLine(cb);

    Color::label(cb, "MESSAGE");
    Color::newLine(cb);
    Color::defaultOutput(cb, error.what());
    Color::newLine(cb);

    if (error.getErrMessage() != "")
    {
        Color::label(cb, "ERRNO");
        Color::newLine(cb);
        Color::defaultOutput(cb, error.getErrMessage());
        Color::newLine(cb);
    }

    Color::appendErrorMark(cb);

    std::cout << cb.build();
}

void OSS::OssOutput::printClock(OSSClock *clock) {
    
    Color::printInfo("OSS", clock->toString());
}




























void OSS::OssOutput::openLogFile()
{
    log_file_.open(file_name_.c_str(), std::ios::out | std::ios::app);
}

void OSS::OssOutput::closeLogFile()
{
    log_file_.close();
}



void OSS::OssOutput::writeLine(const std::string &line) {
    std::cout << line << std::endl;

    if (log_file_.is_open() && file_line_count_ < MAX_LINES) {
        log_file_ << line << std::endl;
        file_line_count_++;
    }
}

void OSS::OssOutput::logProcessLaunch(pid_t pid, OSSClock *clock) {
    writeLine("OSS launched process pid " + std::to_string(pid) + " at time " + clock->toString());
}

void OSS::OssOutput::logProcessRequest(pid_t pid, int resource, OSSClock *clock) {
    writeLine("OSS has detected pid " + std::to_string(pid) + "requesting R" + std::to_string(resource) + " at time " + clock->toString());
}
void OSS::OssOutput::logGrantRequest(pid_t pid, int resource, OSSClock *clock) {
    writeLine("OSS granting pid " + std::to_string(pid) + " request for R" + std::to_string(resource) + " at time " + clock->toString());
}
void OSS::OssOutput::logBlockProcess(pid_t pid, int resource, OSSClock *clock) {
    writeLine("OSS blocking pid " + std::to_string(pid) + " waiting for R" + std::to_string(resource) + " at time " + clock->toString());
}
void OSS::OssOutput::logResourceRelease(pid_t pid, int resource, OSSClock *clock) {
    writeLine("OSS has acknowledged pid " + std::to_string(pid) + " releasing R" + std::to_string(resource) + " at time " + clock->toString());
}
void OSS::OssOutput::logTerminateProcess(pid_t pid, OSSClock *clock){
    writeLine("OSS has acknowledged pid " + std::to_string(pid) + " terminating at time " + clock->toString());
}
void OSS::OssOutput::logUnblockProcess(pid_t pid, int resource, OSSClock *clock) {
    writeLine("OSS unblocked pid " + std::to_string(pid) + " and granted R" + std::to_string(resource) + " at time " + clock->toString());
}











void OSS::OssOutput::cleanUp()
{
    if (log_file_.is_open())
    {
        closeLogFile();
    }
}