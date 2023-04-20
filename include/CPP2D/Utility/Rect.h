#pragma once

#include "../util.h"

namespace cpp2d
{
    template<typename T>
    struct Rect
    {
        T left, top, width, height;
    };

    typedef Rect<float>   FloatRect;
    typedef Rect<int32_t> IntRect;
}