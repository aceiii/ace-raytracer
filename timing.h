#ifndef __TIMING_H__
#define __TIMING_H__

#include <chrono>

class timer {
public:
    typedef std::chrono::duration<double, std::nano> nanoseconds;
    typedef std::chrono::duration<double, std::micro> microseconds;
    typedef std::chrono::duration<double, std::milli> milliseconds;
    typedef std::chrono::duration<double> seconds;

    timer() = default;

    void zero() {
        start = std::chrono::high_resolution_clock::now();
    }

    template<typename T = seconds>
    double duration() const {
        using namespace std::chrono;
        const auto end = high_resolution_clock::now();
        const auto diff = duration_cast<T>(end - start);
        return diff.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
};

#endif//__TIMING_H__
