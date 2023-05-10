#pragma once

#include <vector>

#include <cpp2d/Utility.h>
#include "Allocation.h"

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

    class AttributeBuffer :
        public Utility::NoCopy
    {
    protected:
        Buffers::Allocation* _allocation;

        Binding _binding;
        std::vector<Attribute> _attributes;

    public:
        void setBinding(const Binding& binding);
        Binding getBinding() const;

        virtual void setData(const void* data, CU32& bytes) = 0;

        void pushAttributes(const std::initializer_list<Attribute>& list);
        void pushAttribute(const Attribute& attribute);
        const std::vector<Attribute>& getAttributes() const;

        Graphics::BufferHandle getHandle() const;
    };  
}