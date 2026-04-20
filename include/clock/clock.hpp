#include "time.hpp"
#include <string>
#include <sys/shm.h>
#include "../error/error.hpp"


using namespace std;

class Clock {
private:
    string parent_ = "";

    const int SECOND_TO_NANO = 1000000000;
    const int BUFF_SZ = sizeof(Time);

    string key_ = "";
    int shm_key_ = 0;
    int shm_id_ = -1;
    Time *current_time_ = nullptr;
    
public:
    explicit Clock(string parent, string key);
    Time *getCurrentTime();
    void addTimeToPtrTime(Time *ptr_time, Time add_time);
    void subtractTimeToPtrTime(Time *ptr_time, Time subtract_time);
    void detach();
    void rmid();

    string toString();
};