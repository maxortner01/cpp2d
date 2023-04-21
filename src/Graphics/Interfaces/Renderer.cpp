#include <CPP2D/Graphics.h>

namespace cpp2d
{
namespace Interface
{
    Renderer::Renderer() :  
        _frame_index(0)
    {   }

    void Renderer::submitFrameTime(const uint32_t& fps)
    {
        _last_fps[(_frame_index++) % 10] = fps;
    }

    uint32_t Renderer::getFPS() const
    {
        const uint32_t frame_count = sizeof(_last_fps) / sizeof(uint32_t);
        float fps_sum = 0;
        for (uint32_t i = 0; i < frame_count; i++)
            fps_sum += (float)_last_fps[i];

        return (uint32_t)(fps_sum / (float)frame_count);
    }
}
}