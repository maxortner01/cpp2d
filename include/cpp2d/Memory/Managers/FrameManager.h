#pragma once

#include "StackManager.h"

namespace cpp2d::Memory
{
    template<typename _Allocator>
    class FrameManager :
        public Manager<FrameManager<_Allocator>>
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

        AddrDist offset(void* const * ptr) const override;
        void request(void** ptr, CU32& bytes) override;
        void release(void** ptr) override;

        void resetWrite();
    };

    template<typename _Allocator>
    FrameManager<_Allocator>::FrameManager()
    {   
        _Allocator& allocator = _Allocator::get();
        this->_heap = allocator.allocate(1024 * 100);
        _iterator = this->_heap;
    }

    template<typename _Allocator>
    FrameManager<_Allocator>::~FrameManager()
    {
        if (this->_heap)
        {
            _Allocator& allocator = _Allocator::get();
            allocator.free(this->_heap);
        }
    }

    template<typename _Allocator>
    AddrDist FrameManager<_Allocator>::offset(void* const* ptr) const
    {
        return (U8*)*ptr - (U8*)this->_heap;
    }

    template<typename _Allocator>
    void FrameManager<_Allocator>::request(void** ptr, CU32& bytes) 
    {
        assert(ptr && *ptr);
        *ptr = _iterator;
        _last_alloc.addr = _iterator;
        _last_alloc.size = bytes;

        _iterator = (void*)((U8*)_iterator + bytes);
        assert(offset(&_iterator) < 1024 * 100);
    }

    template<typename _Allocator>
    void FrameManager<_Allocator>::release(void** ptr)
    {
        if (*ptr == _last_alloc.addr)
            _iterator = _last_alloc.addr;

        *ptr = nullptr;
    }

    template<typename _Allocator>
    void FrameManager<_Allocator>::resetWrite()
    {
        _iterator = this->_heap;
    }
}