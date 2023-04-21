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

        template<typename F>
        Vec2<T> operator=(const Vec2<F>& vec) const
        {
            return Vec2<T>(
                (T)vec.x, (T)vec.y
            );
        }

        template<typename F>
        Vec2<T> operator+(const Vec2<F>& vec) const
        {
            return Vec2<T>(
                x + (T)vec.x, y + (T)vec.y
            );
        }

        template<typename F>
        Vec2<T> operator*(const F& val) const
        {
            return Vec2<T>(
                x * (T)val, y * (T)val
            );
        }

        template<typename F>
        Vec2<T> operator-(const Vec2<F>& vec) const
        {
            return Vec2<T>(
                x - (T)vec.x, y - (T)vec.y
            );
        }

        template<typename F>
        Vec2<T> operator/(const F& val) const
        {
            return Vec2<T> (
                x / val, y / val
            );
        }
    };

    typedef Vec2<float>    Vec2f;
    typedef Vec2<uint32_t> Vec2u;
    typedef Vec2<int32_t>  Vec2i;
}