#include <CPP2D/Graphics.h>

namespace cpp2d
{
    QuadRenderer::QuadRenderer() :
        _quad(1)
    {
        const float vertices[] = {
            -0.5,  0.5,
             0.5,  0.5,
             0.5, -0.5,
            -0.5, -0.5
        };

        const uint32_t indices[] = {
            0, 1, 2, 2, 3, 0
        };
        
        _quad[0].setData(&vertices[0], sizeof(float) * 8);
        _quad[0].setAttributeData({ 0, 2 });
        _quad.setIndices(&indices[0], 6);
    }

    void QuadRenderer::render(DrawSurface& surface, Quad& object) const
    {
        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElements(GL_LINE_STRIP, _quad.getIndexCount(), GL_UNSIGNED_INT, _quad.getIndices());
    }
}