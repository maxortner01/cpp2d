#pragma once

#include "../util.h"

namespace cpp2d
{
    class Timer
    {
        double _start_time;

    public:
        Timer();

        double getTime() const;
        void restart();
    };
}