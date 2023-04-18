#pragma once

#include "../../Utility.h"

namespace cpp2d
{
    // I don't like this...
    // But I want to abstract this so that either a window or a 
    // texture/framebuffer can be used
    // So the contents of this class will be fleshed out more once I implement
    // framebuffers
    class DrawSurface
    {
    public:
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual Vec2u getSize() const = 0;

        // Could remove virtual and make this bind(); clear(); unbind()
        virtual void clear() = 0;
    };
}