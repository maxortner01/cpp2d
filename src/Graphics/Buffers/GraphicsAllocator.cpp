#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#   include <vk_mem_alloc.h>
#endif

namespace cpp2d::Buffers
{

    GraphicsAllocator::GraphicsAllocator(Graphics::GDI& gdi)
    {   
        VmaAllocator allocator;
        VmaAllocatorCreateInfo allocator_create {
            .physicalDevice = static_cast<VkPhysicalDevice>(gdi.getSuitablePhysicalDevice()),
            .device = static_cast<VkDevice>(gdi.getCurrentLogicDevice().handle),
            .instance = static_cast<VkInstance>(gdi.getHandle())
        };

        // create the allocator with vma
        VkResult result = vmaCreateAllocator(&allocator_create, &allocator);
        if (result != VK_SUCCESS)
            cpp2dERROR("Error creating graphics allocator.");
        else
            _allocator = static_cast<Graphics::AllocatorHandle>(allocator);

        // register the allocator with the gdi
        //gdi.registerAllocator(this);
    }

    GraphicsAllocator::~GraphicsAllocator()
    {   
        // Go though manifest of pointers and free
        // seems cheap to use STL in a custom allocator... so its TEMP
        std::vector<void*> temp_pointers(pointers.begin(), pointers.end());

        for (U32 i = 0; i < temp_pointers.size(); i++)
            free(temp_pointers[i]);

        vmaDestroyAllocator(static_cast<VmaAllocator>(_allocator));
    }

    /*
    GraphicsAllocatorData* GraphicsAllocator::extractData(const void* ptr) const
    {
        assert(ptr);
        U32 heap_size = *((U32*)ptr - 1);
        U32 data_size = *((U32*)ptr - 2);
        assert(data_size == sizeof(GraphicsAllocatorData));
        auto* data = (GraphicsAllocatorData*)((U8*)ptr - sizeof(U32) * 2 - data_size);

        return data;
    }*/

    void* GraphicsAllocator::allocate(CU32& _bytes)
    {
        cpp2dINFO("Allocating %u bytes on the GPU.", _bytes);
        AllocationHeader alloc_data;

        alloc_data.data_size  = sizeof(GraphicsAllocatorData);
        alloc_data.heap_size = _bytes;

#   ifdef GDI_VULKAN
        VkBufferCreateInfo buffer_create {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size  = _bytes + GraphicsAllocator::HeaderBytes,
            .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        };

        VmaAllocationCreateInfo allocation_create {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
        };

        VkBuffer      _handle;
        VmaAllocation _allocation;
        auto allocator = static_cast<VmaAllocator>(_allocator);

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

        alloc_data.data.buffer     = static_cast<Graphics::BufferHandle>(_handle);
        alloc_data.data.allocation = static_cast<Graphics::AllocationHandle>(_allocation);

        void* data;
        vmaMapMemory(allocator, _allocation, &data);
        std::memcpy(data, &alloc_data, GraphicsAllocator::HeaderBytes);
#   endif

        void* ret = (void*)((U8*)data + GraphicsAllocator::HeaderBytes);
        pointers.push_back(ret);

        return ret;
    }
    
    void GraphicsAllocator::free(void* ptr)
    {
        auto* data = extractData(ptr);

#ifdef GDI_VULKAN
        VkBuffer      buffer     = static_cast<VkBuffer>     (data->data.buffer);
        VmaAllocator  allocator  = static_cast<VmaAllocator> (_allocator);
        VmaAllocation allocation = static_cast<VmaAllocation>(data->data.allocation);
        vmaUnmapMemory(allocator, allocation);
        vmaDestroyBuffer(allocator, buffer, allocation);
#endif

        pointers.erase(
            std::find(pointers.begin(), pointers.end(), ptr)
        );
    }
}