#pragma once

#include "../Util.h"

namespace cpp2d::Graphics
{
    class CPP2D_DLL VertexBuffer
    {
        const GDILogicDevice _device;

        U32 _allocated_bytes;
        BufferHandle     _handle;
        AllocationHandle _allocation;

        Binding _binding;
        std::vector<Attribute> _attributes;

    public:
        VertexBuffer();
        VertexBuffer(const GDILogicDevice& device);
        ~VertexBuffer();

        void setData(const void* data, CU32& byteSize);
        void allocate(CU32& byteSize);
        void free();

        void setBinding(const Binding& binding);
        Binding getBinding() const;

        void pushAttributes(const std::initializer_list<Attribute>& list);
        void pushAttribute(const Attribute& attribute);
        const std::vector<Attribute>& getAttributes() const;

        BufferHandle getHandle() const;
    };
}