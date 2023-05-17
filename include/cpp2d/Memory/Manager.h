#pragma once

#include "../Util.h"
#include "../Utility.h"

namespace cpp2d::Memory
{
    typedef void (*Callback)(void*, U32, void*);

    // This shouldn't be copied, since the manager
    // keeps track of ManagedAllocation pointers...
    // If, for some reason, the ManagedAllocation instance
    // needs to move, this should be destroyed and a new one
    // should be registered with the manager
    struct ManagedAllocation : 
        public Utility::NoCopy
    {
        ManagedAllocation(void* owner);

        void  setOnChanged(Callback callback);
        void  setPointer(void* newPointer, U32 size);
        void* getPointer() const;
        U32   getSize() const;

    private:
        void* _owner;
        U32   _size;
        void* _pointer;
        bool  _changed;
        Callback _callback;
    };
    
    enum class MemoryOwner
    {
        Allocator,
        Manager
    };

    struct Manager :
        public Utility::NoCopy
    {
        Manager(void* owner, const MemoryOwner& ownerType);

        virtual AddrDist getHeapOffset() const { return 0; }
        virtual AddrDist offset(const void* ptr) const { return 0; }
        virtual void     request(ManagedAllocation* ptr, CU32& bytes) = 0;
        virtual void     release(ManagedAllocation* ptr) = 0;

        MemoryOwner ownerType() const;
        Allocator*  getAllocator() const;
        const void* getHeap() const;

    protected:
        ManagedAllocation _heap;
        void* const       _memory_owner;
        const MemoryOwner _owner_type;
    };

    ManagedAllocation::ManagedAllocation(void* owner) :
        _pointer(nullptr),
        _changed(false),
        _callback(nullptr),
        _size(0),
        _owner(owner)
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
        if (_callback) _callback(newPointer, size, _owner);
        _pointer = newPointer;
        _size = size;
    }

    Manager::Manager(void* owner, const MemoryOwner& ownerType) :
        _memory_owner(owner),
        _owner_type(ownerType),
        _heap(this)
    {   }

    MemoryOwner Manager::ownerType() const
    {
        return _owner_type;
    }

    Allocator* Manager::getAllocator() const
    {
        switch (_owner_type)
        {
            case MemoryOwner::Allocator:
            {
                auto* allocator = reinterpret_cast<Allocator*>(_memory_owner);
                return allocator;
            }
            case MemoryOwner::Manager:
            {
                auto* manager = reinterpret_cast<Manager*>(_memory_owner);
                return manager->getAllocator();
            }
        }
    }

    const void* Manager::getHeap() const
    {
        return _heap.getPointer();
    }
}