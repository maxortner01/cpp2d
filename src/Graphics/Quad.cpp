#include <CPP2D/Graphics.h>

#include <vector>
#include <GL/glew.h>

namespace cpp2d
{
    Quad::Quad()  :
        _z(0), _texture_rect({ 0, 0, 1, 1 })
    {

    }

    void Quad::setColor(const Color& color)
    {   
        _color = color;
    }

    float Quad::getZ() const
    {
        return _z;
    }

    FloatRect Quad::getTextureRect() const
    {
        return _texture_rect;
    }

    void Quad::setTextureRect(const FloatRect& rect)
    {
        _texture_rect = rect;
    }

    void Quad::setZ(const float& z)
    {
        _z = z;
    }

    Color Quad::getColor() const
    {
        return _color;
    }
}