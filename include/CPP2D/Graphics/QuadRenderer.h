#pragma once

#include "./Interfaces/Renderer.h"
#include "Quad.h"

namespace cpp2d
{
    struct QuadSceneStruct
    {

    };

    class QuadRenderer : 
        public Renderer<QuadSceneStruct>
    {
        VertexArray _quad;

    public:
        QuadRenderer();

        void render(DrawSurface& surface, const Shader& shader, const Quad& quad) const;
        void render(DrawSurface& surface, const Shader& shader, const Quad*  const objects, const uint32_t& count) const;
        void render(DrawSurface& surface, const Shader& shader, const Quad* const* const objects, const uint32_t& count) const;
    };
}