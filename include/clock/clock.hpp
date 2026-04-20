#include "time.hpp"
#include <string>
#include <sys/shm.h>
#include "../error/error.hpp"


using namespace std;

class Clock {
private:
    string parent_ = "";

    const int NANO_TO_SECOND = 1000000000;
    const int BUFF_SZ = sizeof(Time);

    string key_ = "";
    int shm_key_ = 0;
    int shm_id_ = -1;
    Time *sim_clock_ = nullptr;
    
public:
    explicit Clock(string parent, string key);
    Time *getCurrentTime();
    void addTime(Time &a, Time &b);
    void subtractTime(Time &a, Time &b);
    void detach();
    void rmid();

    string toString();
};