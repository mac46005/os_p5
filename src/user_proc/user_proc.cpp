#include "../../include/user_proc/user_proc.hpp"
#include <ctime>

UserProcess::UserProcess(int argc, char **argv)
{
    pid_ = getpid();
    ppid_ = getppid();

    argument_processor_ = new ArgumentProcessor(argc, argv);

    argument_processor_->process(
        [this](Options options)
        {
            clock_checker_ = new ClockChecker(options.sec, options.nano);
        });

    srand(getpid() ^ time(nullptr));
    msg_manager_ = new MsgManager("msgq.txt", 0644, pid_);
}

bool UserProcess::shouldRequest()
{
    // return (rand() % 100) < 90;

    int held_total = 0;
    for (int i = 0; i < 10; i++)
    {
        held_total += held_resources_[i];
    }

    if (held_total == 0)
    {
        return true;
    }

    return (rand() % 100) < 90;
}

bool UserProcess::hasAnyHeldResources()
{
    for (int i = 0; i < 10; i++)
    {
        if (held_resources_[i] > 0)
        {
            return true;
        }
    }
    return false;
}

int UserProcess::chooseRequestResource()
{

    // may need to change this to see if resource is more than 5
    // return rand() % 10;
    for (int attempts = 0; attempts < 20; attempts++)
    {
        int roll = rand() % 100;
        int resource;

        if (roll < 70)
        {
            int hot[] = {7, 8, 9};
            resource = hot[rand() % 3];
        }
        else
        {
            resource = rand() % 10;
        }

        if (held_resources_[resource] < 5)
        {
            return resource;
        }
    }

    return -1;
}
int UserProcess::chooseReleaseResource()
{
    std::vector<int> owned;
    for (int i = 0; i < 10; i++)
    {
        if (held_resources_[i] > 0)
        {
            owned.push_back(i);
        }
    }

    if (owned.empty())
    {
        return -1;
    }

    return owned[rand() % owned.size()];
}
int UserProcess::run()
{
    try
    {
        Color::printInfo("UserProcess", "running...");
        MsgBuffer recieve{};
        while (true)
        {
            msg_manager_->recieveMessage(
                [this, &recieve](MsgBuffer msg)
                {
                    recieve = msg;

                    // dont forget that parent can force kill you
                },
                0);

            if (recieve.status == GRANTED)
            {
                if (recieve.resource >= 0 && recieve.resource < 10)
                {
                    held_resources_[recieve.resource]++;
                }
                continue;
            }
            else
            {
                if (clock_checker_->isTimeReached())
                {
                    // send message to oss for terminating via time limit
                    Color::printInfo("UserProcess", "Time reached. Terminating...");
                    msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);
                    break;
                }

                if (shouldRequest())
                {
                    int resource = chooseRequestResource();
                    if (resource >= 0)
                    {
                        msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::REQUEST, resource, 0);
                    }
                    else
                    {
                        int release_resource = chooseReleaseResource();
                        if (release_resource >= 0)
                        {
                            held_resources_[release_resource]--;
                            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::RELEASE, release_resource, 0);
                        }
                        else
                        {
                            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);
                            break;
                        }
                    }
                }
                else
                {
                    int resource = chooseReleaseResource();
                    if (resource >= 0)
                    {
                        held_resources_[resource]--;
                        msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::RELEASE, resource, 0);
                    }
                    else
                    {
                        int resource_requested = chooseRequestResource();
                        if (resource_requested >= 0)
                        {
                            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::REQUEST, resource_requested, 0);
                        }
                        else
                        {
                            msg_manager_->sendMessage(ppid_, pid_, ProcessStatus::TERMINATE, -1, 0);
                            break;
                        }
                    }
                }
            }
        }
        Color::printInfo("UserProcess", "Terminating...");

        cleanUp();
    }
    catch (std::exception &e)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void UserProcess::cleanUp()
{
    clock_checker_->cleanUp();
}