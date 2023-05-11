#pragma once

#include "../Util.h"
#include "../Utility.h"

namespace cpp2d::Memory
{
    // There *should* be a singleton instance for each instance class put into this
    // but the Singleton::get() returns the _ChildClass. This way we fake instantiation
    // Lets see if this works before philosophizing...
    template<class _ChildClass, class _Allocator>
    struct Manager :
        public Utility::Singleton<_ChildClass>
    {
        virtual AddrDist offset(void* const * ptr) const = 0;
        virtual void     request(void** ptr, CU32& bytes) = 0;
        virtual void     release(void** ptr) = 0;
    };
}