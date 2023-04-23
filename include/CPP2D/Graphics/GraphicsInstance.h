#pragma once

#include "../Utility.h"

namespace cpp2d
{
    enum class GraphicsState
    {
        Success,
        gl3wFailed
    };

    class GraphicsInstance : 
        public Utility::Singleton<GraphicsInstance>, 
        public Utility::State<GraphicsState>
    {
        bool _init;

    public:
        GraphicsInstance();

        void init();

        bool isInitialized() const;

        void drawInstanced(const uint32_t& indexCount, const uint32_t& instanceCount) const;
    };
}