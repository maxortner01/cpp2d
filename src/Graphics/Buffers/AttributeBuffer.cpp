#include <cpp2d/Graphics.h>

#include <vector>

namespace cpp2d::Buffers
{   
    template<typename _MemManager>
    AddrDist AttributeBuffer<_MemManager>::offset() const
    {
        _MemManager& allocator = _MemManager::get();
        return allocator.offset(&_data);
    }

    template<typename _MemManager>
    void AttributeBuffer<_MemManager>::setBinding(const Binding& binding)
    {
        _binding = binding;
    }

    template<typename _MemManager>
    Binding AttributeBuffer<_MemManager>::getBinding() const
    {
        return _binding;
    }
    
    template<typename _MemManager>
    void AttributeBuffer<_MemManager>::setData(const void* data, CU32& bytes)
    {
        _MemManager& allocator = _MemManager::get();

        if (bytes > _allocated_size || !data)
        {
            if (bytes > _allocated_size)
                allocator.release(&_data);

            allocator.request(&_data, bytes);
            _allocated_size = bytes;
        }

        std::memcpy(_data, data, bytes);
    }

    template<typename _MemManager>
    void AttributeBuffer<_MemManager>::pushAttributes(const std::initializer_list<Attribute>& list)
    {
        std::copy(list.begin(), list.end(), std::back_inserter(_attributes));
    }

    template<typename _MemManager>
    void AttributeBuffer<_MemManager>::pushAttribute(const Attribute& attribute)
    {   
        _attributes.push_back(attribute);
    }

    template<typename _MemManager>
    const std::vector<Attribute>& AttributeBuffer<_MemManager>::getAttributes() const
    {
        return _attributes;
    }
    
    //typedef cpp2d::Buffers::AttributeBuffer<cpp2d::Memory::StackManager<cpp2d::Buffers::GraphicsAllocator, cpp2d::VertexArray>> test;
}