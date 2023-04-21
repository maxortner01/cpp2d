#pragma once

#include <vector>

#include "../../Utility.h"
#include "../Shader.h"

namespace cpp2d
{
    class DrawSurface;

namespace Interface
{
    class Renderer
    {
        uint32_t _frame_index;
        uint32_t _last_fps[10];
    
    protected:
        Utility::Timer _frame_timer;
        void submitFrameTime(const uint32_t& fps);

    public:
        Renderer();

        uint32_t getFPS() const;
    };
}

    template<typename T>
    class Renderer :
        public Interface::Renderer
    {
    protected:

        T _scene_struct;

    public:
        void begin(const T& sceneStruct)
        {
            _frame_timer.restart();
            std::memcpy(&_scene_struct, &sceneStruct, sizeof(T));
        }

        void end()
        {
            const uint32_t fps = (uint32_t)(1.0 / _frame_timer.getTime());
            submitFrameTime(fps);
        }
    };
}