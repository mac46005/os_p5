#include <vector>
#include "pcb_queue.hpp"
#include "oss_clock.hpp"
#include "resource_manager.hpp"
#include "oss_output.hpp"

namespace OSS {
    struct PCBInfo {
        int max_process_count_ = 0;
        int max_simultaneous_count_ = 0;
        int process_count_ = 0;
        int simultaneous_count_ = 0;
        const int MAX_PCB = 18;
        int pcb_count_ = 0;
        inline bool hasOpenPCBSlot(){
            if (pcb_count_ < MAX_PCB){
                return true;
            }
            return false;
        }
        inline bool isSimulCountReached() {
            if (simultaneous_count_ == max_simultaneous_count_) {
                return true;
            }
            return false;
        }
        inline bool isProcCountReached() {
            if (process_count_ == max_process_count_) {
                return true;
            }
            return false;
        }
    };

    class Scheduler {
    private:
        OSSClock *oss_clock_;
        ResourceManager *resource_manager_;
        OssOutput *oss_output_;



        PCBInfo pcb_info_{};

        PCBQueue *pcb_ready_queue_;
        std::vector<PCB> pcb_blocked_list;
        std::vector<PCB> completed_processes;
        
        PCB *current_process_running_ = new PCB{.pid = -1};
        PCB createPCB(pid_t pid);
        void forkProcess();

        
    public:
        explicit Scheduler(int max_proc, int max_simul, OSSClock *oss_clock, ResourceManager *resource_manager, OssOutput *oss_output);
        
        bool stillHaveChildrenToLaunch();
        bool stillHaveChildrenInSystem();

        void launchChildrenIfAble();
        void launchProcess();
        void canUnblockAdBlockedProcess();
        void hasProcessInReadyQueue();
         
        void updatePCBState();
        PCBQueue *getReadyQueue();
        std::vector<PCB> getBlockedList();
    };
}