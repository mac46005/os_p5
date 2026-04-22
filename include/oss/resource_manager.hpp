#pragma once
#include <array>
#include <vector>
#include "pcb.hpp"
namespace OSS
{
    // STATS
    // total number of resources requested
    // total times deadlock detection was ran
    // how many processes terminate due to deadlock detection
    // percentage of requests that were granted versus the amount of total requests
    // USE RESOURCE DETECTION ALGORITHM
    class ResourceManager {
    private:
        static constexpr int RESOURCE_COUNT = 10;
        static constexpr int INSTANCES_PER_RESOURCE = 5;


        std::array<int, RESOURCE_COUNT> total_{};
        std::array<int, RESOURCE_COUNT> available_{};

        int total_requests_ = 0;
        int granted_immediately_ = 0;
        int deadlock_runs_ = 0;
        int deadlock_kills_ = 0;

    public:
        ResourceManager();

        bool canGrant(int resource) const;
        bool grant(PCB &pcb, int resource);
        bool releaseOne(PCB &pcb, int resource);
        void releaseAll(PCB &pcb);

        const std::array<int, RESOURCE_COUNT> &getAvailable() const;
        const std::array<int, RESOURCE_COUNT> &getTotal() const;

        int getTotalRequests() const;
        int getGrantedImmediately() const;
        int getDeadlockRuns() const;
        int getDeadlockKills() const;

        void incrementDeadlockRuns();
        void incrementDeadlockKills();

        bool detecedDeadlock(const std::vector<PCB> &blocked_list) const;
        std::vector<int> getDeadlockIndices(const std::vector<PCB> &blocked_list) const;
    };
}