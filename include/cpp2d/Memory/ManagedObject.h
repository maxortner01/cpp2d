#pragma once

#include "Manager.h"

namespace cpp2d::Memory
{
    template<class _Object>
    class ManagedObject
    {
        Memory::Manager* const _manager;
        ManagedAllocation _allocation;

    public:
        template<typename... Args>
        ManagedObject(Memory::Manager* manager, Args... args);

        // Take the new manager and create a new allocation,
        // then we want to copy the memory directory over
        //template<typename _OtherManager>
        ManagedObject(const ManagedObject<_Object>& object);

        virtual ~ManagedObject();

        _Object& object() const;
    };

    template<class _Object>
    template<typename... Args>
    ManagedObject<_Object>::ManagedObject(Memory::Manager* manager, Args... args) : 
        _manager(manager),
        _allocation(manager)
    {
        // Get the pointer for the new object an initialize the object at
        // that address
        _manager->request(&_allocation, sizeof(_Object));
        new(_allocation.getPointer()) _Object(args...);
    }

    template<class _Object>
    //template<typename _OtherManager>
    ManagedObject<_Object>::ManagedObject(const ManagedObject<_Object>& object) :
        _manager(object._manager),
        _allocation(object._manager)
    {
        _manager->request(&_allocation, sizeof(_Object));
        std::memcpy(_allocation.getPointer(), object._allocation.getPointer(), sizeof(_Object));
    }

    template<class _Object>
    ManagedObject<_Object>::~ManagedObject()
    {
        _manager->release(&_allocation);
    }   

    template<class _Object>
    _Object& ManagedObject<_Object>::object() const
    {
        return *(_Object*)_allocation.getPointer();
    }
}