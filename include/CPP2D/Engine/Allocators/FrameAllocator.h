#pragma once

#include "../../Memory.h"
#include "../../Utility.h"

namespace cpp2d::Allocators
{
    class FrameAllocator : 
        public Utility::Singleton<FrameAllocator>,
        public Memory::StackAllocator
    {
    public:
        FrameAllocator();

        using Memory::StackAllocator::allocate;
        using Memory::StackAllocator::clear;
    };
}