#include <CPP2D/Graphics.h>

#include <vector>
#include <GL/glew.h>

namespace cpp2d
{
    Quad::Quad() :
        _vertex_array(1)
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
        
        _vertex_array[0].setData(&vertices[0], sizeof(float) * 8);
        _vertex_array[0].setAttributeData({ 0, 2 });
        _vertex_array.setIndices(&indices[0], 6);
    }

    void Quad::draw(DrawSurface& surface) const
    {
        _vertex_array.bind();
        _vertex_array[0].bind();
        glDrawElements(GL_LINE_STRIP, _vertex_array.getIndexCount(), GL_UNSIGNED_INT, _vertex_array.getIndices());
    }
}