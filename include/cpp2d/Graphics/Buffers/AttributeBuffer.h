#pragma once

#include <vector>

#include <cpp2d/Utility.h>
#include <cpp2d/Memory.h>

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

    class AttributeBuffer
    {
        Binding _binding;
        std::vector<Attribute> _attributes;

    public:
        AttributeBuffer();
        virtual ~AttributeBuffer();

        void    setBinding(const Binding& binding);
        Binding getBinding() const;

        void pushAttributes(const std::initializer_list<Attribute>& list);
        void pushAttribute(const Attribute& attribute);
        const std::vector<Attribute>& getAttributes() const;
    };  

    class MutableBuffer
    {
    protected:
        Memory::ManagedAllocation _data;

    public:
        U32      getAllocatedSize() const;
        virtual AddrDist offset() const = 0;

        virtual void setData(const void* data, CU32& bytes) = 0;
    };

    struct BufferArrayInstance :
        public AttributeBuffer,
        public MutableBuffer
    {   };

    template<typename _Object, typename _MemManager>
    class TypeBuffer :
        public BufferArrayInstance
    {
    public:
        typedef _MemManager MemoryManager;

        static void bind(const Graphics::FrameObject<Graphics::FrameData>* frameData);

        TypeBuffer();
        ~TypeBuffer();

        AddrDist offset() const override;

        void setData(const void* data, CU32& bytes) override;
    };

    template<typename _MemManager>
    class TypeBuffer<U32, _MemManager> :
        public BufferArrayInstance
    {
    public:
        typedef _MemManager MemoryManager;

        static void bind(const Graphics::FrameObject<Graphics::FrameData>* frameData);

        TypeBuffer();
        ~TypeBuffer();

        AddrDist offset() const override;

        void setData(const void* data, CU32& bytes) override;
    };

    // Implementation    
    AttributeBuffer::AttributeBuffer() 
    {   }

    AttributeBuffer::~AttributeBuffer()
    {   }

    void AttributeBuffer::setBinding(const Binding& binding)
    { _binding = binding; }

    Binding AttributeBuffer::getBinding() const
    { return _binding; }

    void AttributeBuffer::pushAttributes(const std::initializer_list<Attribute>& list)
    { std::copy(list.begin(), list.end(), std::back_inserter(_attributes)); }
    
    void AttributeBuffer::pushAttribute(const Attribute& attribute)
    { _attributes.push_back(attribute); }

    const std::vector<Attribute>& AttributeBuffer::getAttributes() const
    { return _attributes; }
    
    U32 MutableBuffer::getAllocatedSize() const
    { return _data.getSize(); }

    template<typename _Object, typename _MemManager>
    void TypeBuffer<_Object, _MemManager>::bind(const Graphics::FrameObject<Graphics::FrameData>* frameData)
    {
        // call vulkan bind
    }

    template<typename _MemManager>
    void TypeBuffer<U32, _MemManager>::bind(const Graphics::FrameObject<Graphics::FrameData>* frameData)
    {
        // call vulkan bind
    }

    template<typename _Object, typename _MemManager>
    TypeBuffer<_Object, _MemManager>::TypeBuffer()
    {   
        static_assert(std::is_base_of<Memory::Manager<_MemManager>, _MemManager>::value);
    }

    template<typename _MemManager>
    TypeBuffer<U32, _MemManager>::TypeBuffer()
    {   
        static_assert(std::is_base_of<Memory::Manager<_MemManager>, _MemManager>::value);

        setBinding(Binding {
            .index = 0,
            .stride = sizeof(U32)
        });
    }

    template<typename _Object, typename _MemManager>
    TypeBuffer<_Object, _MemManager>::~TypeBuffer()
    {
        if (_data.getPointer())
        {
            _MemManager& manager = _MemManager::get();
            manager.release(&_data);
        }
    }

    template<typename _MemManager>
    TypeBuffer<U32, _MemManager>::~TypeBuffer()
    {
        if (_data.getPointer())
        {
            _MemManager& manager = _MemManager::get();
            manager.release(&_data);
        }
    }
    
    template<typename _Object, typename _MemManager>
    AddrDist TypeBuffer<_Object, _MemManager>::offset() const
    {
        _MemManager& allocator = _MemManager::get();
        //return allocator.offset(_data.getPointer());
        return _MemManager::Allocator::get().offset(allocator.getHeap()) + allocator.offset(_data.getPointer());
    }

    template<typename _MemManager>
    AddrDist TypeBuffer<U32, _MemManager>::offset() const
    {
        _MemManager& allocator = _MemManager::get();
        return _MemManager::Allocator::get().offset(allocator.getHeap()) + allocator.offset(_data.getPointer());
    }
    
    template<typename _Object, typename _MemManager>
    void TypeBuffer<_Object, _MemManager>::setData(const void* data, CU32& bytes)
    {
        _MemManager& allocator = _MemManager::get();

        if (bytes > getAllocatedSize() || !_data.getPointer())
        {
            if (bytes > getAllocatedSize() && _data.getPointer())
                allocator.release(&_data);

            allocator.request(&_data, bytes);
        }

        std::memcpy(_data.getPointer(), data, bytes);
    }

    template<typename _MemManager>
    void TypeBuffer<U32, _MemManager>::setData(const void* data, CU32& bytes)
    {
        _MemManager& allocator = _MemManager::get();

        if (bytes > getAllocatedSize() || !data)
        {
            if (bytes > getAllocatedSize() && _data.getPointer())
                allocator.release(&_data);

            allocator.request(&_data, bytes);
        }

        std::memcpy(_data.getPointer(), data, bytes);
    }
}