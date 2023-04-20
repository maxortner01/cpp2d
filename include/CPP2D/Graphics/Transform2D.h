#pragma once

#include "Shader.h"
#include "../Utility.h"

namespace cpp2d
{
    class Transform2D
    {
        Vec2f _scale;
        Vec2f _center;
        Vec2f _position;
        float _rotation;

    public:
        Transform2D();

        void passToShader(const Shader& shader) const;

        void setPosition(const Vec2f& position);
        void setRotation(const float& rotation);
        void setCenter(const Vec2f& center);
        void setScale(const Vec2f& scale);

        Vec2f& getPositionRef();
        float& getRotationRef();
        Vec2f& getCenterRef();
        Vec2f& getScaleRef();

        Vec2f getPosition() const;
        float getRotation() const;
        Vec2f getCenter() const;
        Vec2f getScale() const;
    };
}