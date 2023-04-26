#include <cpp2d/Engine.h>

namespace cpp2d::Allocators
{
    // Let's allocate 1024 kilobytes for each frame
    FrameAllocator::FrameAllocator() :
        Memory::StackAllocator(1024 * 1000)
    {

    }
}