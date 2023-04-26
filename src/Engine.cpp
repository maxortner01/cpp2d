#include <entt/entt.hpp>

#include <iostream>
#include <cstdlib>

#include "./Engine/Animation.cpp"
#include "./Engine/Application.cpp"
#include "./Engine/Scene.cpp"
#include "./Engine/System.cpp"
#include "./Engine/Entity.cpp"

#include "./Engine/Allocators/FrameProfiler.cpp"
#include "./Engine/Allocators/FrameAllocator.cpp"

#include "./Engine/Systems/SpriteRenderer.cpp"

void * operator new(std::size_t n) 
{
    return cpp2d::Allocators::FrameProfiler::get().allocate(n);
}
void operator delete(void* p) 
{
    cpp2d::Allocators::FrameProfiler::get().free(p);
}