#pragma once

#include "../Util.h"

#include <vector>

namespace cpp2d::Graphics
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
}