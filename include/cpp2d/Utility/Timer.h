#pragma once

#include <chrono>
#include "../Util.h"

namespace cpp2d::Utility
{
    typedef decltype(std::chrono::high_resolution_clock::now()) Timestamp;
#ifdef __APPLE__
    typedef std::__1::chrono::duration<long long, std::__1::nano> Duration;
#else
    typedef std::chrono::duration<long long, std::nano> Duration;
#endif

    class Timer;

    struct Time
    {
        Time(Duration duration);

        R64 seconds() const;
        R64 milliseconds() const;
        R64 microseconds() const;
        R64 nanoseconds() const;
        
    private:
        Duration _duration;
    };

    class Timer
    {
        Timestamp _start_time;
    public:
        Timer();

        void restart();

        Time getTime() const;
    };


    // Implementations
    Time::Time(Duration duration) :
        _duration(duration)
    {   }

    R64 Time::seconds() const
    {
        return (R64)std::chrono::duration_cast<std::chrono::nanoseconds>(_duration).count() / pow(10, 9);
    }

    R64 Time::milliseconds() const
    {
        return (R64)std::chrono::duration_cast<std::chrono::nanoseconds>(_duration).count() / 1000.0;
    }

    R64 Time::microseconds() const
    {
        return (R64)std::chrono::duration_cast<std::chrono::nanoseconds>(_duration).count() / pow(10, 6);
    }

    R64 Time::nanoseconds() const
    {
        return (R64)std::chrono::duration_cast<std::chrono::nanoseconds>(_duration).count();
    }

    Timer::Timer()
    {
        restart();
    }

    void Timer::restart()
    {
        _start_time = std::chrono::high_resolution_clock::now();
    }

    Time Timer::getTime() const
    {
        Timestamp end_time = std::chrono::high_resolution_clock::now();

        return Time(end_time - _start_time);
    }
}