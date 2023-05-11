#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#   include <vk_mem_alloc.h>
#endif

namespace cpp2d::Buffers
{
    /*
    Allocation::Allocation() :
        _handle(nullptr),
        _allocation(nullptr),
        _allocation_size(0),
        _data(nullptr)
    {   }*/

    /*
    void Allocation::setData(const void* data, CU32& bytes, CU32& offset)
    {
        assert(bytes + offset < _allocation_size);
        void* _ptr = (void*)((char*)_data + offset);
        std::memcpy(_ptr, data, bytes);
    }*/

    GraphicsAllocator::GraphicsAllocator()
    {   }

    GraphicsAllocator::~GraphicsAllocator()
    {   }

    void** GraphicsAllocator::allocate(CU32& bytes)
    {
        GraphicsAllocatorData* ptr = (GraphicsAllocatorData*)std::malloc(sizeof(GraphicsAllocatorData));
        std::memset(ptr, 0, sizeof(GraphicsAllocatorData));

#   ifdef GDI_VULKAN
        VkBufferCreateInfo buffer_create {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size  = bytes,
            .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        };

        VmaAllocationCreateInfo allocation_create {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
        };

        VkBuffer      _handle;
        VmaAllocation _allocation;
        auto allocator = static_cast<VmaAllocator>(Graphics::GDI::get().getAllocator());

        VkResult result = vmaCreateBuffer(
            allocator,
            &buffer_create,
            &allocation_create,
            &_handle, 
            &_allocation,
            nullptr
        );

        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Vertex buffer allocation failed.");
            return nullptr;
        }

        ptr->buffer     = static_cast<Graphics::BufferHandle>(_handle);
        ptr->allocation = static_cast<Graphics::AllocationHandle>(_allocation);
        ptr->allocation_size = bytes;
        vmaMapMemory(allocator, _allocation, &ptr->data);
#   endif

        return (void**)ptr;
    }
    
    void GraphicsAllocator::free(void** ptr)
    {
        assert(ptr);
        GraphicsAllocatorData* data = (GraphicsAllocatorData*)(ptr);

#ifdef GDI_VULKAN
        VmaAllocator  allocator  = static_cast<VmaAllocator> (Graphics::GDI::get().getAllocator());
        VmaAllocation allocation = static_cast<VmaAllocation>(data->allocation);
        VkBuffer      buffer     = static_cast<VkBuffer>(data->buffer);
        vmaUnmapMemory(allocator, allocation);
        vmaDestroyBuffer(allocator, buffer, allocation);
#endif

        data->buffer     = nullptr;
        data->allocation = nullptr;
        data->data       = nullptr;
        std::free(ptr);
    }

    /*

    void GraphicsAllocator::reallocate(CU32& bytes, Allocation* allocation)
    {
        assert(allocation);
        if (!allocation->_data) return allocate(bytes, allocation);

        if (!bytes) free(allocation);

        CU32  copy_size = std::min(bytes, allocation->_allocation_size);
        void* temp_data = std::malloc(copy_size); // sub out for 'cpu allocator'->allocate class

        std::memcpy(temp_data, allocation->_data, copy_size);
        free(allocation);

        allocate(bytes, allocation);
        std::memcpy(allocation->_data, temp_data, copy_size);
        std::free(temp_data); // sub out for 'cpu allocator'->free class

        allocation->_allocation_size = bytes;
    }

    void GraphicsAllocator::allocate(CU32& bytes, Allocation* allocation)
    {
        assert(allocation);

#   ifdef GDI_VULKAN
        VkBufferCreateInfo buffer_create {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size  = bytes,
            .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        };

        VmaAllocationCreateInfo allocation_create {
            .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
        };

        VkBuffer      _handle;
        VmaAllocation _allocation;
        auto allocator = static_cast<VmaAllocator>(Graphics::GDI::get().getAllocator());

        VkResult result = vmaCreateBuffer(
            allocator,
            &buffer_create,
            &allocation_create,
            &_handle, 
            &_allocation,
            nullptr
        );

        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Vertex buffer allocation failed.");
            return;
        }

        allocation->_handle     = static_cast<Graphics::BufferHandle>(_handle);
        allocation->_allocation = static_cast<Graphics::AllocationHandle>(_allocation);
        allocation->_allocation_size = bytes;
        vmaMapMemory(allocator, _allocation, &allocation->_data);
#   endif
    }

    void GraphicsAllocator::free(Allocation* _allocation)
    {
        assert(_allocation && _allocation->_data);

        VmaAllocator  allocator  = static_cast<VmaAllocator> (Graphics::GDI::get().getAllocator());
        VmaAllocation allocation = static_cast<VmaAllocation>(_allocation->_allocation);
        VkBuffer      buffer     = static_cast<VkBuffer>(_allocation->_handle);
        vmaUnmapMemory(allocator, allocation);
        vmaDestroyBuffer(allocator, buffer, allocation);
        _allocation->_handle     = nullptr;
        _allocation->_allocation = nullptr;
        _allocation->_data       = nullptr;
    }*/
}