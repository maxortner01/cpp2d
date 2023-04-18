#pragma once

#include "./Interfaces/Renderer.h"
#include "Quad.h"

namespace cpp2d
{
    class QuadRenderer : 
        public Renderer<Quad>
    {
        VertexArray _quad;

    public:
        QuadRenderer();

        void render(DrawSurface& surface, const Shader& shader, const Quad& quad) const;
        void render(DrawSurface& surface, const Shader& shader) const override;
    };
}