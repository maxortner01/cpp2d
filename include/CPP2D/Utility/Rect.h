#pragma once

#include "../util.h"

namespace cpp2d
{
    template<typename T>
    struct Rect
    {
        T left, top, width, height;
    };

    typedef Rect<R32> FloatRect;
    typedef Rect<U32> UnsignedRect;
    typedef Rect<I32> IntRect;
}