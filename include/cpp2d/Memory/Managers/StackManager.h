#pragma once

#include "../Manager.h"

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
    template<typename _Allocator, class _InstanceClass = BaseStack>
    class StackManager :
        public Manager<StackManager<_Allocator, _InstanceClass>, _Allocator>
    {
        void* _iterator;

        StackManager();
        ~StackManager();

    public:
        friend class Utility::Singleton<StackManager<_Allocator, _InstanceClass>>;

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
    };

    template<typename _Allocator, class _InstanceClass>
    StackManager<_Allocator, _InstanceClass>::StackManager()
    {
        _Allocator& allocator = _Allocator::get();
        this->_heap= allocator.allocate(1000); // Some constant starting value
        _iterator = this->_heap;
    }

    template<typename _Allocator, class _InstanceClass>
    StackManager<_Allocator, _InstanceClass>::~StackManager()
    {
        if (this->_heap)
        {
            _Allocator& allocator = _Allocator::get();
            allocator.free(this->_heap);
            this->_heap = nullptr;
        }
    }

    template<typename _Allocator, class _InstanceClass>
    AddrDist StackManager<_Allocator, _InstanceClass>::offset(void* const * ptr) const
    {
        return (char*)*ptr - (char*)this->_heap;
    }

    template<typename _Allocator, class _InstanceClass>
    void StackManager<_Allocator, _InstanceClass>::request(void** ptr, CU32& bytes) 
    {
        *ptr = _iterator;
        _iterator = (void*)((char*)_iterator + bytes);
    }

    template<typename _Allocator, class _InstanceClass>
    void StackManager<_Allocator, _InstanceClass>::release(void** ptr)
    {
        *ptr = nullptr;
    }
}