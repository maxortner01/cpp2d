#include <cpp2d/Graphics.h>

namespace cpp2d
{
    Transform2D::Transform2D()
    {
        setPosition(Vec2f());
        setRotation(0);
        setCenter(Vec2f(0, 0));
        setScale(Vec2f(1, 1));
    }

    /*
    void Transform2D::passToShader(const Shader& shader) const
    {
        shader.setUniform("rotation", getRotation());
        shader.setUniform("position", getPosition());
        shader.setUniform("center", getCenter());
        shader.setUniform("scale", getScale());
    }*/

    void Transform2D::setPosition(const Vec2f& position)
    {
        _position = position;
    }

    void Transform2D::setRotation(const float& rotation)
    {
        _rotation = rotation;
    }

    void Transform2D::setCenter(const Vec2f& center)
    {
        _center = center;
    }

    void Transform2D::setScale(const Vec2f& scale)
    {
        _scale = scale;
    }

    Vec2f& Transform2D::getPositionRef()
    {
        return _position;
    }

    float& Transform2D::getRotationRef()
    {
        return _rotation;
    }

    Vec2f& Transform2D::getCenterRef()
    {
        return _center;
    }

    Vec2f& Transform2D::getScaleRef()
    {
        return _scale;
    }

    Vec2f Transform2D::getPosition() const
    {
        return _position;
    }

    float Transform2D::getRotation() const
    {
        return _rotation;
    }

    Vec2f Transform2D::getCenter() const
    {
        return _center;
    }

    Vec2f Transform2D::getScale() const
    {
        return _scale;
    }
}