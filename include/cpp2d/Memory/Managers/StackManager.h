#pragma once

#include "../Managers/HeapManager.h"
#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

#define SIZE 100

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
        public Manager<StackManager<_Allocator, _SubManager, _InstanceClass>>
    {
        U32   _allocated_size;
        //void* _iterator;

        std::vector<U32>    _chunk_sizes;
        std::vector<void**> _chunks;

        void* _iterator() const; 

        StackManager();
        ~StackManager();

        static constexpr bool IS_ALLOCATOR = std::is_base_of<Memory::Allocator<_Allocator>, _Allocator>::value;
        static constexpr bool IS_MANAGER   = std::is_base_of<Memory::Manager<_Allocator>,   _Allocator>::value;

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

        void print() const;
    };

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    void* StackManager<_Allocator, _SubManager, _InstanceClass>::_iterator() const
    {
        U32 bytes = 0;
        for (U32 i = 0; i < _chunks.size(); i++)
            bytes += _chunk_sizes[i];

        return (void*)((U8*)this->_heap + bytes);
    }
    

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    StackManager<_Allocator, _SubManager, _InstanceClass>::StackManager()
    {
        // we require that the allocator is either a legit allocator 
        // or a manager we get pointers from, but also that the submanager (which
        // is used for the temporary copy storage)
        static_assert(IS_ALLOCATOR || IS_MANAGER && std::is_base_of<Memory::Manager<_SubManager>, _SubManager>::value);

        _Allocator& allocator = _Allocator::get();
        if constexpr (IS_ALLOCATOR) this->_heap = allocator.allocate(SIZE);
        if constexpr (IS_MANAGER)   allocator.request(&this->_heap, SIZE);

        _allocated_size = SIZE;
        //_iterator = this->_heap;
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    StackManager<_Allocator, _SubManager, _InstanceClass>::~StackManager()
    {
        if (this->_heap)
        {
            _Allocator& allocator = _Allocator::get();
            if constexpr (IS_ALLOCATOR) allocator.free(this->_heap);
            if constexpr (IS_MANAGER)   allocator.release(&this->_heap);
            this->_heap = nullptr;
        }
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    AddrDist StackManager<_Allocator, _SubManager, _InstanceClass>::offset(void* const * ptr) const
    {
        return (char*)*ptr - (char*)this->_heap;
    }

    // things aren't working...
    // we need a way to tell the sub manager that its heap position has
    // changed, that way, it can go through its pointers and update their
    // new positions... currently the only time these positions are updated
    // are when the manager itself reallocates, not when its parent reallocates
    //
    // we can fix this by letting the heap pointer be within a container class
    // overload the operator= function to overwrite the pointer and to set a flag
    // that its been changed, something like that

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    void StackManager<_Allocator, _SubManager, _InstanceClass>::request(void** ptr, CU32& bytes) 
    {
        void* next_iterator_pos = (void*)((char*)_iterator() + bytes);
        if (offset(&next_iterator_pos) >= _allocated_size)
        {
            _Allocator& allocator = _Allocator::get();

            // reallocate
            void* temp = nullptr;
            CU32 bytes_to_copy = bytesUsed();
            if (bytes_to_copy)
            {
                _SubManager& subManager = _SubManager::get();

                subManager.request(&temp, bytes_to_copy);
                std::memcpy(temp, this->_heap, bytes_to_copy);
            }

            CU32 new_size = (U32)(_allocated_size * 1.25) + bytes;

            if constexpr (IS_ALLOCATOR) 
            {
                allocator.free(this->_heap);
                this->_heap = allocator.allocate(new_size);
            }
            if constexpr (IS_MANAGER)   
            {
                allocator.release(&this->_heap);
                allocator.request(&this->_heap, new_size);
            }
            _allocated_size = new_size;

            if (temp)
            {
                _SubManager& subManager = _SubManager::get();
                std::memcpy(this->_heap, temp, bytes_to_copy);
                subManager.release(&temp);
            }

            void* iterator = _iterator();
            for (U32 i = 0; i < _chunks.size(); i++)
            {
                void** chunk = _chunks[i];
                U32    size  = _chunk_sizes[i];

                *chunk = iterator;
                iterator = (void*)((U8*)iterator + size);
            }
        }

        *ptr = _iterator();
        //_iterator = (void*)((char*)_iterator + bytes);

        // if the iterator surpasses the size, we need to request a new block with a new size,
        // relocating all the pointers
        //assert(offset(&iterator) < _allocated_size);
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
        CU32 bytes_to_copy = (U32)((U8*)_iterator() - (U8*)*ptr) - chunk_size;
        
        //_iterator = (void*)((U8*)_iterator - chunk_size);

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
        void* iterator = _iterator();
        return offset(&iterator);
    }

    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    void StackManager<_Allocator, _SubManager, _InstanceClass>::print() const
    {
        U32 used = 0;
        for (U32 i = 0; i < _chunks.size(); i++)
        {
            used += _chunk_sizes[i];
            for (U32 j = 0; j < _chunk_sizes[i]; j++) std::cout << "|";
        }
        
        CU32 remaining = _allocated_size - bytesUsed();
        for (U32 i = 0; i < remaining; i++) std::cout << "-";
        std::cout << "\n";
        std::cout << "Used: " << bytesUsed() << " bytes.  Remaining: " << remaining << " bytes.\n";
    }
}