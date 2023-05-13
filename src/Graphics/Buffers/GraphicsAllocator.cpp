#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#   include <vk_mem_alloc.h>
#endif

namespace cpp2d::Buffers
{

    GraphicsAllocator::GraphicsAllocator()
    {   
        auto& gdi = Graphics::GDI::get();

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
        Graphics::GDI::get().registerAllocator(this);
    }

    GraphicsAllocator::~GraphicsAllocator()
    {   
        cpp2dERROR("Destroying allocator");

        // Go though manifest of pointers and free

        vmaDestroyAllocator(static_cast<VmaAllocator>(_allocator));
    }

    GraphicsAllocatorData* GraphicsAllocator::extractData(const void* ptr) const
    {
        assert(ptr);
        U32 heap_size = *((U32*)ptr - 1);
        U32 data_size = *((U32*)ptr - 2);
        assert(data_size == sizeof(GraphicsAllocatorData));
        auto* data = (GraphicsAllocatorData*)((U8*)ptr - sizeof(U32) * 2 - data_size);

        return data;
    }

    void* GraphicsAllocator::allocate(CU32& _bytes)
    {
        cpp2dINFO("Allocating %u bytes on the GPU.", _bytes);
        struct 
        {
            GraphicsAllocatorData data{0};
            U32 size;
            U32 bytes;
        } alloc_data;

        alloc_data.size  = sizeof(GraphicsAllocatorData);
        alloc_data.bytes = _bytes;

        //GraphicsAllocatorData* ptr = (GraphicsAllocatorData*)std::malloc(sizeof(GraphicsAllocatorData));
        //std::memset(ptr, 0, sizeof(GraphicsAllocatorData));

#   ifdef GDI_VULKAN
        VkBufferCreateInfo buffer_create {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size  = _bytes + sizeof(alloc_data),
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

        //ptr->buffer     = static_cast<Graphics::BufferHandle>(_handle);
        //ptr->allocation = static_cast<Graphics::AllocationHandle>(_allocation);
        //ptr->allocation_size = bytes;
        alloc_data.data.buffer     = static_cast<Graphics::BufferHandle>(_handle);
        alloc_data.data.allocation = static_cast<Graphics::AllocationHandle>(_allocation);

        void* data;
        vmaMapMemory(allocator, _allocation, &data);
        std::memcpy(data, &alloc_data, sizeof(alloc_data));
#   endif

        return (void*)((U8*)data + sizeof(alloc_data));
    }
    
    void GraphicsAllocator::free(void* ptr)
    {
        //GraphicsAllocatorData* data = (GraphicsAllocatorData*)(ptr);
        auto* data = extractData(ptr);

#ifdef GDI_VULKAN
        VmaAllocator  allocator  = static_cast<VmaAllocator> (_allocator);
        VmaAllocation allocation = static_cast<VmaAllocation>(data->allocation);
        VkBuffer      buffer     = static_cast<VkBuffer>(data->buffer);
        vmaUnmapMemory(allocator, allocation);
        vmaDestroyBuffer(allocator, buffer, allocation);
#endif

        //data->buffer     = nullptr;
        //data->allocation = nullptr;
        //data->data       = nullptr;
        std::free(
            (void*)data
        );
    }
}