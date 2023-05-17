#pragma once

#include "StackManager.h"

namespace cpp2d::Memory
{
    template<typename _Allocator>
    class FrameManager :
        public Manager
    {
        struct 
        {
            void* addr;
            U32   size;
        } _last_alloc;
        
        void* _iterator;

        FrameManager();
        ~FrameManager();

    public:
        friend class Utility::Singleton<FrameManager<_Allocator>>;

        AddrDist offset(const void* ptr) const override;
        void request(ManagedAllocation* ptr, CU32& bytes) override;
        void release(ManagedAllocation* ptr) override;

        void resetWrite();
    };
#define SECOND_SIZE 1024 * 100

    template<typename _Allocator>
    FrameManager<_Allocator>::FrameManager()
    {
        _Allocator& allocator = _Allocator::get();
        this->_heap.setPointer(allocator.allocate(SECOND_SIZE), SECOND_SIZE);
        _iterator = this->_heap.getPointer();
    }

    template<typename _Allocator>
    FrameManager<_Allocator>::~FrameManager()
    {
        if (this->_heap)
        {
            _Allocator& allocator = _Allocator::get();
            allocator.free(this->_heap.getPointer());
        }
    }

    template<typename _Allocator>
    AddrDist FrameManager<_Allocator>::offset(const void* ptr) const
    {
        return (U8*)ptr - (U8*)this->_heap.getPointer();
    }

    template<typename _Allocator>
    void FrameManager<_Allocator>::request(ManagedAllocation* ptr, CU32& bytes) 
    {
        assert(ptr);
        ptr->setPointer(_iterator, bytes);
        _last_alloc.addr = _iterator;
        _last_alloc.size = bytes;

        _iterator = (void*)((U8*)_iterator + bytes);
        assert(offset(_iterator) < SECOND_SIZE);
    }

    template<typename _Allocator>
    void FrameManager<_Allocator>::release(ManagedAllocation* ptr)
    {
        if (ptr->getPointer() == _last_alloc.addr)
            _iterator = _last_alloc.addr;

        ptr->setPointer(nullptr, 0);
    }

    template<typename _Allocator>
    void FrameManager<_Allocator>::resetWrite()
    {
        _iterator = this->_heap.getPointer();
    }
}