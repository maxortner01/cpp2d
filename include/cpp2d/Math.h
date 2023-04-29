#pragma once

#include <cstring>
#include <initializer_list>

#include "Util.h"

namespace cpp2d
{
#pragma region VEC2

    template<typename T>
    struct Vec2
    {
        union
        {
            T m[2];
            struct { T x, y; };
        };

        Vec2();
        Vec2(T _x, T _y);
        Vec2(std::initializer_list<T> list);

        template<typename F>
        Vec2<F> operator+(const Vec2<F>& vec) const;

        template<typename F>
        Vec2<F> operator*(const F& scale) const;
    };

    // Implementations
    template<typename T>
    Vec2<T>::Vec2() :
        x(0), y(0)
    {   }

    template<typename T>
    Vec2<T>::Vec2(T _x, T _y) :
        x(_x), y(_y)
    {   }

    template<typename T>
    Vec2<T>::Vec2(std::initializer_list<T> list) 
    {
        assert(list.size() == 2);
        std::memcpy(m, list.begin(), sizeof(T) * 2);
    }

    template<typename T>
    template<typename F>
    Vec2<F> Vec2<T>::operator+(const Vec2<F>& vec) const
    {
        return Vec2<F>(
            x + vec.x,
            y + vec.y
        );
    }

    template<typename T>
    template<typename F>
    Vec2<F> Vec2<T>::operator*(const F& scale) const
    {
        return Vec2<F>(
            x * scale,
            y * scale
        );
    }

    // Typedefs
    typedef Vec2<R32> Vec2f;
    typedef Vec2<U32> Vec2u;
    typedef Vec2<I32> Vec2i;

#pragma endregion VEC2
}