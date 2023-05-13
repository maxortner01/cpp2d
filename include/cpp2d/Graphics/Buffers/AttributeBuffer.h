#pragma once

#include <vector>

#include <cpp2d/Utility.h>

namespace cpp2d::Buffers
{
    struct Binding
    {
        U32 index;
        U32 stride;
    };

    struct Attribute
    {
        U32 binding;
        U32 location;
        U32 element_count;
        U32 offset;
    };

    struct AttributeFrame
    {
        std::vector<Binding> bindings;
        std::vector<Attribute> attributes;
    };

    template<typename _MemManager>
    class AttributeBuffer :
        public Utility::NoCopy
    {
        void* _data;
        U32   _allocated_size;

        Binding _binding;
        std::vector<Attribute> _attributes;

    public:
        U32      getAllocatedSize() const;
        AddrDist offset() const;

        void setBinding(const Binding& binding);
        Binding getBinding() const;

        void setData(const void* data, CU32& bytes);

        void pushAttributes(const std::initializer_list<Attribute>& list);
        void pushAttribute(const Attribute& attribute);
        const std::vector<Attribute>& getAttributes() const;
    };  

    // Implementation

    template<typename _MemManager>
    U32 AttributeBuffer<_MemManager>::getAllocatedSize() const
    {
        return _allocated_size;
    }
    
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
}