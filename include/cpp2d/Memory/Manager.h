#pragma once

#include "../Util.h"
#include "../Utility.h"

namespace cpp2d::Memory
{
    typedef void (*Callback)(void*, U32);

    // This shouldn't be copied, since the manager
    // keeps track of ManagedAllocation pointers...
    // If, for some reason, the ManagedAllocation instance
    // needs to move, this should be destroyed and a new one
    // should be registered with the manager
    struct ManagedAllocation : 
        public Utility::NoCopy
    {
        ManagedAllocation();

        void setOnChanged(Callback callback);
        void setPointer(void* newPointer, U32 size);
        void* getPointer() const;
        U32 getSize() const;

    private:
        U32   _size;
        void* _pointer;
        bool  _changed;
        Callback _callback;
    };

    template<class _ChildClass>
    struct Manager :
        public Utility::Singleton<_ChildClass>
    {
        virtual AddrDist offset(const void* ptr) const = 0;
        virtual void     request(ManagedAllocation* ptr, CU32& bytes) = 0;
        virtual void     release(ManagedAllocation* ptr) = 0;

        const void* getHeap() const;

    protected:
        ManagedAllocation _heap;
    };

    ManagedAllocation::ManagedAllocation() :
        _pointer(nullptr),
        _changed(false),
        _callback(nullptr),
        _size(0)
    {   }

    U32 ManagedAllocation::getSize() const
    {
        return _size;
    }

    // used for free in heap allocator... no where else!
    void* ManagedAllocation::getPointer() const
    {
        return _pointer;
    }

    void ManagedAllocation::setOnChanged(Callback callback)
    {
        _callback = callback;
    }

    void ManagedAllocation::setPointer(void* newPointer, U32 size)
    {
        if (_callback) _callback(newPointer, size);
        _pointer = newPointer;
        _size = size;
    }

    template<class _ChildClass>
    const void* Manager<_ChildClass>::getHeap() const
    {
        return _heap.getPointer();
    }
}