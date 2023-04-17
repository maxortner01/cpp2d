#pragma once

#include "../Utility.h"

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
        virtual Vec2f getSize() const = 0;
    };
}