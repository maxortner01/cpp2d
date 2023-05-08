#include <cpp2d/Graphics.h>

#include <vk_mem_alloc.h>

namespace cpp2d::Graphics
{
    VertexBuffer::VertexBuffer() :
        VertexBuffer(GDI::get().getCurrentLogicDevice())
    {   }

    VertexBuffer::VertexBuffer(const GDILogicDevice& device) :
        _device(device),
        _handle(nullptr),
        _allocation(nullptr),
        _allocated_bytes(0)
    {
    
    }

    VertexBuffer::~VertexBuffer()
    {
        if (_allocation)
            free();
    }

    void VertexBuffer::setData(const void* data, CU32& byteSize)
    {
        if (_allocated_bytes != byteSize) allocate(byteSize);

        auto allocator  = static_cast<VmaAllocator> (GDI::get().getAllocator(_device.handle));
        auto allocation = static_cast<VmaAllocation>(_allocation);

        void* _data;
        vmaMapMemory(allocator, allocation, &_data);
        std::memcpy(_data, data, byteSize);
        vmaUnmapMemory(allocator, allocation);
    }

    void VertexBuffer::allocate(CU32& byteSize)
    {
        if (_allocation || _handle) free();
        VkBufferCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = byteSize,
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        };

        VmaAllocationCreateInfo alloc_info {
            .usage = VMA_MEMORY_USAGE_CPU_TO_GPU
        };

        VkBuffer handle;
        VmaAllocation allocation;

        auto allocator = static_cast<VmaAllocator>(GDI::get().getAllocator(_device.handle));
        VkResult result = vmaCreateBuffer(
            allocator,
            &create_info,
            &alloc_info,
            &handle, 
            &allocation,
            nullptr
        );

        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Vertex buffer allocation failed.");
            return;
        }

        _handle     = static_cast<BufferHandle>(handle);
        _allocation = static_cast<AllocationHandle>(allocation);
        _allocated_bytes = byteSize;
    }

    void VertexBuffer::free()
    {
        assert(_allocation);
        VmaAllocator  allocator  = static_cast<VmaAllocator> (GDI::get().getAllocator(_device.handle));
        VmaAllocation allocation = static_cast<VmaAllocation>(_allocation);
        VkBuffer      buffer     = static_cast<VkBuffer>(_handle);
        vmaDestroyBuffer(allocator, buffer, allocation);
        _handle = nullptr;
        _allocation = nullptr;
    }

    void VertexBuffer::setBinding(const Binding& binding)
    {
        _binding = binding;
    }

    Binding VertexBuffer::getBinding() const
    {
        return _binding;
    }

    void VertexBuffer::pushAttributes(const std::initializer_list<Attribute>& list)
    {
        std::copy(list.begin(), list.end(), std::back_inserter(_attributes));
    }

    void VertexBuffer::pushAttribute(const Attribute& attribute)
    {   
        _attributes.push_back(attribute);
    }

    const std::vector<Attribute>& VertexBuffer::getAttributes() const
    {
        return _attributes;
    }

    BufferHandle VertexBuffer::getHandle() const
    {
        return _handle;
    }
}