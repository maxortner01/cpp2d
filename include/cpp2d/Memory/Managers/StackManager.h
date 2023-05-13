#pragma once

#include "../Managers/HeapManager.h"
#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

#define SIZE 100000

namespace cpp2d::Memory
{
    enum class BaseStack {};

    /**
     * @class StackManager
     * @brief Manages memory allocation and deallocation for a stack of objects.
     *
     * This class provides an interface for requesting and releasing memory from a stack, using the provided allocator.
     *
     * @tparam _Allocator The allocator to use for memory management.
     * @tparam _InstanceClass The class to use as the base class for stack instances.
     */
    template<typename _Allocator, typename _SubManager = HeapManager, class _InstanceClass = BaseStack>
    class StackManager :
        public Manager<StackManager<_Allocator, _SubManager, _InstanceClass>, _Allocator>
    {
        void* _iterator;

        std::vector<U32>    _chunk_sizes;
        std::vector<void**> _chunks;

        StackManager();
        ~StackManager();

    public:
        friend class Utility::Singleton<StackManager<_Allocator, _SubManager, _InstanceClass>>;

        /**
         * @brief Calculates the offset of a memory address from the start of the stack.
         *
         * This method calculates the offset of the specified memory address from the start of the stack. The offset is returned as an AddrDist value.
         *
         * @param ptr A pointer to the memory address for which to calculate the offset.
         * @return An AddrDist value representing the offset of the memory address from the start of the stack.
         */
        AddrDist offset(void* const* ptr) const override;

        /**
         * @brief Requests memory from the stack.
         *
         * This method is called to request memory from the stack, which will be a part of the memory allocated by the provided allocator.
         *
         * @param ptr A pointer to a pointer to the requested memory.
         * @param bytes The size, in bytes, of the requested memory.
         */
        void request(void** ptr, CU32& bytes) override;

        /**
         * @brief Releases memory from the stack.
         *
         * This method is called to release memory that was previously allocated from the stack using the request method.
         *
         * @param ptr A pointer to a pointer to the memory to release.
         */
        void release(void** ptr) override;

        U32 bytesUsed() const;
    };

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    StackManager<_Allocator, _SubManager, _InstanceClass>::StackManager()
    {
        _Allocator& allocator = _Allocator::get();
        this->_heap= allocator.allocate(SIZE); // Some constant starting value
        _iterator = this->_heap;
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    StackManager<_Allocator, _SubManager, _InstanceClass>::~StackManager()
    {
        if (this->_heap)
        {
            _Allocator& allocator = _Allocator::get();
            allocator.free(this->_heap);
            this->_heap = nullptr;
        }
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    AddrDist StackManager<_Allocator, _SubManager, _InstanceClass>::offset(void* const * ptr) const
    {
        return (char*)*ptr - (char*)this->_heap;
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    void StackManager<_Allocator, _SubManager, _InstanceClass>::request(void** ptr, CU32& bytes) 
    {
        *ptr = _iterator;
        _iterator = (void*)((char*)_iterator + bytes);

        assert(offset(&_iterator) < SIZE);
        _chunks.push_back(ptr);
        _chunk_sizes.push_back(bytes);
    }

    // Here we defrag by moving the pointers (after the given object) 
    // back the amount of bytes the released object is
    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    void StackManager<_Allocator, _SubManager, _InstanceClass>::release(void** ptr)
    {
        assert(ptr && *ptr);
        CU32 pointer_index = std::distance(_chunks.begin(), std::find(_chunks.begin(), _chunks.end(), ptr));

        CU32 chunk_size    = _chunk_sizes[pointer_index];
        CU32 bytes_to_copy = (U32)((U8*)_iterator - (U8*)*ptr) - chunk_size;
        
        _iterator = (void*)((U8*)_iterator - chunk_size);

        if (bytes_to_copy)
        {
            _SubManager& subManager = _SubManager::get();

            void* temp;
            subManager.request(&temp, bytes_to_copy);
            std::memcpy(temp, (void*)((U8*)*ptr + chunk_size), bytes_to_copy);
            std::memcpy(*ptr, temp, bytes_to_copy);
            subManager.release(&temp);
        }

        for (
            auto ptr_it = std::find(_chunks.begin(), _chunks.end(), ptr) + 1;
            ptr_it != _chunks.end();
            ptr_it++
        )
            **ptr_it = (void*)((U8*)**ptr_it - chunk_size);

        _chunks.erase(_chunks.begin() + pointer_index);
        _chunk_sizes.erase(_chunk_sizes.begin() + pointer_index);

        *ptr = nullptr;
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    U32 StackManager<_Allocator, _SubManager, _InstanceClass>::bytesUsed() const
    {
        return offset(&_iterator);
    }
}