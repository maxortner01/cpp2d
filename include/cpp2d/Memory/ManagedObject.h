#pragma once

#include "Manager.h"

namespace cpp2d::Memory
{
    template<class _Object, class _Manager>
    class ManagedObject
    {
        ManagedAllocation allocation;

    public:
        template<typename... Args>
        ManagedObject(Args... args);

        // Take the new manager and create a new allocation,
        // then we want to copy the memory directory over
        //template<typename _OtherManager>
        ManagedObject(const ManagedObject<_Object, _Manager>& object);

        virtual ~ManagedObject();

        _Object& object() const;
    };

    template<class _Object, class _Manager>
    template<typename... Args>
    ManagedObject<_Object, _Manager>::ManagedObject(Args... args)
    {
        static_assert(std::is_base_of<Manager<_Manager>, _Manager>::value);
        _Manager& manager = _Manager::get();

        // Get the pointer for the new object an initialize the object at
        // that address
        manager.request(&allocation, sizeof(_Object));
        new(allocation.getPointer()) _Object(args...);
    }

    template<class _Object, class _Manager>
    //template<typename _OtherManager>
    ManagedObject<_Object, _Manager>::ManagedObject(const ManagedObject<_Object, _Manager>& object)
    {
        static_assert(std::is_base_of<Manager<_Manager>, _Manager>::value);
        _Manager& manager = _Manager::get();

        manager.request(&allocation, sizeof(_Object));
        std::memcpy(allocation.getPointer(), object.allocation.getPointer(), sizeof(_Object));
    }

    template<class _Object, class _Manager>
    ManagedObject<_Object, _Manager>::~ManagedObject()
    {
        _Manager& manager = _Manager::get();
        manager.release(&allocation);
    }   

    template<class _Object, class _Manager>
    _Object& ManagedObject<_Object, _Manager>::object() const
    {
        return *(_Object*)allocation.getPointer();
    }
}