#include "../clock/clock.hpp"

class ClockChecker {
private:
    Clock *clock_;
    Time time_limit_;

public:
    explicit ClockChecker(int max_sec, int max_nano);
    bool isTimeReached();
    void cleanUp();
};