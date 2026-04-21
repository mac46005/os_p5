#include "../../include/oss/resource_manager.hpp"

OSS::ResourceManager::ResourceManager() {
    total_.fill(INSTANCES_PER_RESOURCE);
    available_.fill(INSTANCES_PER_RESOURCE);
}

bool OSS::ResourceManager::canGrant(int resource) const {
    if (resource < 0 || resource >= RESOURCE_COUNT) return false;
    return available_[resource] > 0;
}

bool OSS::ResourceManager::grant(PCB &pcb, int resource) {
    total_requests_++;

    if (!canGrant(resource)) {
        return false;
    }

    available_[resource]--;
    pcb.resource_allocated[resource]++;
    pcb.requested_resource = -1;
    granted_immediately_++;
    return true;
}

bool OSS::ResourceManager::releaseOne(PCB &pcb, int resource) {
    if (resource < 0 || resource >= RESOURCE_COUNT) {
        return false;
    }

    if (pcb.resource_allocated[resource] <= 0) {
        return false;
    }

    pcb.resource_allocated[resource]--;
    available_[resource]++;
    return true;
}

void OSS::ResourceManager::releaseAll(PCB &pcb) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        available_[i] += pcb.resource_allocated[i];
        pcb.resource_allocated[i] = 0;
    }
    pcb.requested_resource = -1;
}

const std::array<int, 10> &OSS::ResourceManager::getAvailable() const {
    return available_;
}

const std::array<int, 10> &OSS::ResourceManager::getTotal() const {
    return total_;
}

int OSS::ResourceManager::getTotalRequests() const {
    return total_requests_;
}

int OSS::ResourceManager::getGrantedImmediately() const {
    return granted_immediately_;
}

int OSS::ResourceManager::getDeadlockRuns() const {
    return deadlock_runs_;
}

int OSS::ResourceManager::getDeadlockKills() const {
    return deadlock_kills_;
}

void OSS::ResourceManager::incrementDeadlockRuns() {
    deadlock_runs_++;
}

void OSS::ResourceManager::incrementDeadlockKills() {
    deadlock_kills_++;
}

std::vector<int> OSS::ResourceManager::getDeadlockIndices(const std::vector<PCB> &blocked_list) const {
    const int m = RESOURCE_COUNT;
    const int n = static_cast<int>(blocked_list.size());

    if (n == 0) {
        return {};
    }

    std::vector<int> work(available_.begin(), available_.end());
    std::vector<bool> finish(n, false);

    bool progress = true;
    while (progress) {
        progress = false;
        for (int p = 0; p < n; p++) {
            if (finish[p]) {
                continue;
            }

            int req_res = blocked_list[p].requested_resource;
            bool can_finish = false;

            if (req_res >= 0 && req_res < m) {
                can_finish = (work[req_res] >= 1);
            }

            if (can_finish) {
                finish[p] = true;
                progress = true;

                for (int r = 0; r < m; r++) {
                    work[r] += blocked_list[p].resource_allocated[r];
                }
            }
        }
    }

    std::vector<int> deadlocked;

    for (int p = 0; p < n; p++) {
        if (!finish[p]) {
            deadlocked.push_back(p);
        }
    }

    return deadlocked;
}

bool OSS::ResourceManager::detecedDeadlock(const std::vector<PCB> &blocked_list) const {
    return !getDeadlockIndices(blocked_list).empty();
}