#pragma once

#include "../Managers/HeapManager.h"
#include "../Allocators/HeapAllocator.h"
#include "../Manager.h"

#define SIZE 1024 * 10

namespace cpp2d::Memory
{
    class StackManager :
        public Manager
    {
        void* const _sub_manager;

        U32   _padding;
        U32   _alignment;
        U32   _allocated_size;
        void* _iterator;

        std::vector<U32>    _chunk_sizes;
        std::vector<ManagedAllocation*> _chunks;

    public:
        StackManager(void* memoryOwner, void* subManager, const MemoryOwner& ownerType, CU32& alignment = 0);
        ~StackManager();

        AddrDist getHeapOffset() const;
        AddrDist offset(const void* ptr) const override;
        void request(ManagedAllocation* ptr, CU32& bytes) override;
        void release(ManagedAllocation* ptr) override;

        U32 bytesUsed() const;
        void print() const;
        void updateAllocations(void* newPtr, U32 size);
        static void test(void* newptr, U32 size, void* instance);
    };

    /*
    template<typename _Allocator, typename _SubManager, class _InstanceClass>
    void* StackManager<_Allocator, _SubManager, _InstanceClass>::_iterator() const
    {
        U32 bytes = 0;
        for (U32 i = 0; i < _chunks.size(); i++)
            bytes += _chunk_sizes[i];

        return (void*)((U8*)this->_heap + bytes);
    }*/

#pragma region DEFINES

#define ALLOCATE_HEAP(size)  \
        switch (_owner_type) \
        { \
        case MemoryOwner::Allocator: \
            { \
                auto* allocator = reinterpret_cast<Allocator*>(_memory_owner); \
                this->_heap.setPointer(allocator->allocate(size), size); \
                break; \
            } \
         \
        case MemoryOwner::Manager: \
            { \
                auto* manager = reinterpret_cast<Manager*>(_memory_owner); \
                manager->request(&this->_heap, size); \
                break; \
            } \
        } 

#define FREE_HEAP  \
        switch (_owner_type) \
        { \
        case MemoryOwner::Allocator: \
            { \
                auto* allocator = reinterpret_cast<Allocator*>(_memory_owner); \
                allocator->free(this->_heap.getPointer()); \
                break; \
            } \
         \
        case MemoryOwner::Manager: \
            { \
                auto* manager = reinterpret_cast<Manager*>(_memory_owner); \
                manager->release(&this->_heap); \
                break; \
            } \
        } 

#pragma endregion DEFINES

    void StackManager::updateAllocations(void* newptr, U32 size)
    {
        U32 offset = 0; 
        switch (_owner_type)
        {
            case MemoryOwner::Allocator:
            {
                auto* allocator = reinterpret_cast<Allocator*>(_memory_owner);
                offset = allocator->headerSize();
                break;
            }
            case MemoryOwner::Manager:
            {
                auto* manager = reinterpret_cast<Manager*>(_memory_owner);
                if (!_alignment) break;
                offset = manager->getHeapOffset() + manager->offset(newptr);
                break;
            }
        }

        _padding = (_alignment?_alignment - (offset % _alignment):0);
        if (_padding == _alignment) _padding = 0;

        _iterator = (void*)((U8*)newptr + _padding);
        for (U32 i = 0; i < _chunks.size(); i++)
        {
            _chunks[i]->setPointer(_iterator, _chunk_sizes[i]);
            _iterator = (void*)((U8*)_iterator + _chunk_sizes[i]);
        }

        _allocated_size = size;
    }

    void StackManager::test(void* newPtr, U32 size, void* instance)
    {
        auto* stack_instance = reinterpret_cast<StackManager*>(instance);
        stack_instance->updateAllocations(newPtr, size);
    }

    StackManager::StackManager(void* memoryOwner, void* subManager, const MemoryOwner& ownerType, CU32& alignment) :
        Manager(memoryOwner, ownerType),
        _sub_manager(subManager),
        _alignment(alignment)
    {
        U32 offset = 0; 
        switch (_owner_type)
        {
            case MemoryOwner::Allocator:
            {
                auto* allocator = reinterpret_cast<Allocator*>(_memory_owner);
                this->_heap.setPointer(allocator->allocate(SIZE), SIZE);
                offset = allocator->headerSize();
                break;
            }

            case MemoryOwner::Manager:
            {
                auto* manager = reinterpret_cast<Manager*>(_memory_owner);
                manager->request(&this->_heap, SIZE);
                if (!alignment) break;
                offset = manager->getHeapOffset() + manager->offset(this->_heap.getPointer());
                break;
            }
        }
              
        this->_heap.setOnChanged(StackManager::test);
        
        _padding = (_alignment?_alignment - (offset % _alignment):0);
        if (_padding == _alignment) _padding = 0;

        _allocated_size = SIZE;
        _iterator = (void*)((U8*)this->_heap.getPointer() + _padding);
    }

    StackManager::~StackManager()
    {
        if (this->_heap.getPointer())
        {
            FREE_HEAP;            
            this->_heap.setPointer(nullptr, 0);
        }
    }
    
    AddrDist StackManager::getHeapOffset() const
    {
        switch (_owner_type)
        {
            case MemoryOwner::Allocator:
                {
                    auto* allocator = reinterpret_cast<Allocator*>(_memory_owner);
                    return allocator->headerSize();   
                }
                
            case MemoryOwner::Manager:
                {
                    auto* manager = reinterpret_cast<Manager*>(_memory_owner);
                    return manager->getHeapOffset() + manager->offset(
                        (void*)((U8*)this->_heap.getPointer() + _padding)
                    );
                }
        }
    }

    AddrDist StackManager::offset(const void* ptr) const
    {
        return (U8*)ptr - (U8*)(this->_heap.getPointer());
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
    //
    // Fixing this now... added the ManagedAllocation container struct

    void StackManager::request(ManagedAllocation* ptr, CU32& bytes) 
    {
        void* next_iterator_pos = (void*)((char*)_iterator + bytes);
        if (offset(next_iterator_pos) >= _allocated_size)
        {
            auto* subManager = reinterpret_cast<Manager*>(_sub_manager);

            // reallocate
            ManagedAllocation temp(subManager);
            CU32 bytes_to_copy = bytesUsed();
            if (bytes_to_copy)
            {
                subManager->request(&temp, bytes_to_copy);
                std::memcpy(temp.getPointer(), this->_heap.getPointer(), bytes_to_copy);
            }

            CU32 new_size = (U32)(_allocated_size * 1.25) + bytes;

            FREE_HEAP;
            ALLOCATE_HEAP(new_size);
            //_allocated_size = new_size;

            if (temp.getPointer())
            {
                std::memcpy(this->_heap.getPointer(), temp.getPointer(), bytes_to_copy);
                subManager->release(&temp);
            }

            /*
            _iterator = this->_heap.getPointer() + _paddo;
            for (U32 i = 0; i < _chunks.size(); i++)
            {
                ManagedAllocation* chunk = _chunks[i];
                U32 size  = _chunk_sizes[i];

                chunk->setPointer(_iterator, size);
                _iterator = (void*)((U8*)_iterator + size);
            }*/
        }

        ptr->setPointer(_iterator, bytes);
        _iterator = (void*)((char*)_iterator + bytes);

        _chunks.push_back(ptr);
        _chunk_sizes.push_back(bytes);
    }

    // Here we defrag by moving the pointers (after the given object) 
    // back the amount of bytes the released object is
    void StackManager::release(ManagedAllocation* ptr)
    {
        assert(ptr && ptr->getPointer());
        CU32 pointer_index = std::distance(_chunks.begin(), std::find(_chunks.begin(), _chunks.end(), ptr));

        CU32 chunk_size    = _chunk_sizes[pointer_index];
        CU32 bytes_to_copy = (U32)((U8*)_iterator - (U8*)ptr->getPointer()) - chunk_size;
        
        _iterator = (void*)((U8*)_iterator - chunk_size);

        if (bytes_to_copy)
        {
            auto* subManager = reinterpret_cast<Manager*>(_sub_manager);

            ManagedAllocation temp(subManager);
            subManager->request(&temp, bytes_to_copy);
            std::memcpy(temp.getPointer(), (void*)((U8*)ptr->getPointer() + chunk_size), bytes_to_copy);
            std::memcpy(ptr->getPointer(), temp.getPointer(), bytes_to_copy);
            subManager->release(&temp);
        }

        U32 index = pointer_index + 1;
        for (
            auto ptr_it = std::find(_chunks.begin(), _chunks.end(), ptr) + 1;
            ptr_it != _chunks.end();
            ptr_it++
        )
            (*ptr_it)->setPointer((void*)((U8*)(*ptr_it)->getPointer() - chunk_size), _chunk_sizes[index++]);

        _chunks.erase(_chunks.begin() + pointer_index);
        _chunk_sizes.erase(_chunk_sizes.begin() + pointer_index);

        ptr->setPointer(nullptr, 0);
    }

    U32 StackManager::bytesUsed() const
    {
        return offset(_iterator);
    }

    void StackManager::print() const
    {
        U32 used = 0;
        for (U32 i = 0; i < _chunk_sizes.size(); i++)
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