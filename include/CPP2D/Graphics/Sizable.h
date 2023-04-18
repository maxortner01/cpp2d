#pragma once

#include "../Utility.h"

namespace cpp2d
{
    template<typename T>
    class Sizable
    {
        T _size;

    public:
        Sizable(const T& size);

        void  setSize(const T& size);
        T getSize() const;
    };

    typedef Sizable<Vec2f> FloatSizable;    
    typedef Sizable<Vec2u> UnsignedSizable; 
};