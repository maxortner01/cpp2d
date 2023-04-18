#pragma once

#include "../util.h"

namespace cpp2d
{
    template<typename T>
    struct Vec2
    {
        T x, y;

        Vec2(const T& _x = 0, const T& _y = 0) :
            x(_x), y(_y)
        {   }
    };

    typedef Vec2<float>    Vec2f;
    typedef Vec2<uint32_t> Vec2u;
}