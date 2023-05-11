#pragma once

#include <cpp2d/Util.h>
#include <cpp2d/Utility.h>

namespace cpp2d::Memory
{
    // There will be a singletone instance for every
    // type of base class put here
    template<typename _ChildClass>
    struct Allocator :
        public Utility::NoCopy,
        public Utility::Singleton<_ChildClass>
    {
        virtual void** allocate(CU32&) = 0;
        virtual void free(void**) = 0;
    };
}