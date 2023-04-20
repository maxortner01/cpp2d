#pragma once

#include "../util.h"

namespace cpp2d
{
namespace Utility
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
}