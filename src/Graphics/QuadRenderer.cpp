#include <CPP2D/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    QuadRenderer::QuadRenderer() :
        _quad(2)
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
        
        // Instance vertex data
        _quad[0].setData(&vertices[0], sizeof(float) * 8);
        _quad[0].setAttributeData(AttributeData(0, 2));

        // Instanced Transform Data
        _quad[1].setData(nullptr, 0);
        _quad[1].setAttributeData(AttributeData(1, 2, true));
        _quad.setIndices(&indices[0], 6);
    }

    void QuadRenderer::render(DrawSurface& surface) const
    {
        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElements(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0);
    }
}