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

    class MutableBuffer :
        public Utility::NoCopy
    {
    protected:
        Memory::Manager* const    _manager;
        Memory::ManagedAllocation _data;

    public:
        MutableBuffer(Memory::Manager* manager);
        virtual ~MutableBuffer();

        U32      getAllocatedSize() const;

        virtual AddrDist getManagerOffset() const { return 0; }
        virtual AddrDist offset() const = 0;

        virtual void setData(const void* data, CU32& bytes) = 0;
    };

    struct BufferArrayInstance :
        public AttributeBuffer,
        public MutableBuffer
    {   
        using MutableBuffer::MutableBuffer;
    };

    template<typename _Object>
    class TypeBuffer :
        public BufferArrayInstance,
        public Utility::NoCopy
    {
    public:
        static void bind(const Memory::ManagedObject<Graphics::FrameData>* frameData);

        TypeBuffer(Memory::Manager* manager);

        AddrDist getManagerOffset() const override;
        AddrDist offset() const override;

        void setData(const void* data, CU32& bytes) override;
    };

    template<>
    class TypeBuffer<U32> :
        public BufferArrayInstance,
        public Utility::NoCopy
    {
    public:
        static void bind(const Memory::ManagedObject<Graphics::FrameData>* frameData);

        TypeBuffer(Memory::Manager* manager);

        AddrDist getManagerOffset() const override;
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

    MutableBuffer::MutableBuffer(Memory::Manager* manager) :
        _manager(manager),
        _data(manager)
    {   }

    MutableBuffer::~MutableBuffer()
    {
        _manager->release(&_data);
    }
    
    U32 MutableBuffer::getAllocatedSize() const
    { return _data.getSize(); }

    template<typename _Object>
    void TypeBuffer<_Object>::bind(const Memory::ManagedObject<Graphics::FrameData>* frameData)
    {
        // call vulkan bind
    }

    void TypeBuffer<U32>::bind(const Memory::ManagedObject<Graphics::FrameData>* frameData)
    {
        // call vulkan bind
    }

    template<typename _Object>
    TypeBuffer<_Object>::TypeBuffer(Memory::Manager* manager) :
        BufferArrayInstance(manager)
    {   }

    TypeBuffer<U32>::TypeBuffer(Memory::Manager* manager) :
        BufferArrayInstance(manager)
    {   
        setBinding(Binding {
            .index = 0,
            .stride = sizeof(U32)
        });
    }

    template<typename _Object>
    AddrDist TypeBuffer<_Object>::getManagerOffset() const
    {
        //return allocator.offset(_data.getPointer());
        //return _MemManager::Allocator::get().offset(allocator.getHeap()); // <-------------------------
        return _manager->getHeapOffset();
    }

    AddrDist TypeBuffer<U32>::getManagerOffset() const
    {
        return _manager->getHeapOffset();
    }
    
    template<typename _Object>
    AddrDist TypeBuffer<_Object>::offset() const
    {
        return _manager->offset(_data.getPointer());
    }

    AddrDist TypeBuffer<U32>::offset() const
    {
        return _manager->offset(_data.getPointer());
    }
    
    template<typename _Object>
    void TypeBuffer<_Object>::setData(const void* data, CU32& bytes)
    {
        if (bytes > getAllocatedSize() || !_data.getPointer())
        {
            if (bytes > getAllocatedSize() && _data.getPointer())
                _manager->release(&_data);

            _manager->request(&_data, bytes);
        }

        std::memcpy(_data.getPointer(), data, bytes);
    }

    void TypeBuffer<U32>::setData(const void* data, CU32& bytes)
    {
        if (bytes > getAllocatedSize() || !_data.getPointer())
        {
            if (bytes > getAllocatedSize() && _data.getPointer())
                _manager->release(&_data);

            _manager->request(&_data, bytes);
        }

        std::memcpy(_data.getPointer(), data, bytes);
    }
}