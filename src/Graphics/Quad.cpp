#include <CPP2D/Graphics.h>

#include <vector>
#include <GL/glew.h>

namespace cpp2d
{
    void Quad::setColor(const Color& color)
    {   
        _color = color;
    }

    Color Quad::getColor() const
    {
        return _color;
    }
}