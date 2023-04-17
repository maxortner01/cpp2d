#include <CPP2D/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    struct Transform {
        Vec2f position, scale, center;
        float rotation;
    };
    
    QuadRenderer::QuadRenderer() :
        _quad(3)
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
        _quad[0].setAttributeData({ 0, 2 });
        _quad.setIndices(&indices[0], 6);

        // Instanced Transform Data
        _quad[1].setDynamic(true);
        _quad[1].setData(nullptr, 0);
        uint32_t sizes[] = { 2, 2, 2, 1 };
        for (uint32_t i = 1; i < 5; i++)
            _quad[1].setAttributeData({ i, sizes[i - 1], true, sizeof(Transform), (i - 1) * (uint32_t)sizeof(Vec2f) });

        _quad[2].setDynamic(true);
        _quad[2].setData(nullptr, 0);
        _quad[2].setAttributeData({ 5, 4, true });
    }

    void QuadRenderer::render(DrawSurface& surface, const Shader& shader) const
    {
        shader.bind();
        shader.setUniform("aspectRatio", surface.getSize().x / surface.getSize().y);

        std::vector<Color>     colors(objects.size());
        std::vector<Transform> transform_data(objects.size());
        std::memset(&transform_data[0], 0, sizeof(Transform) * objects.size());
        for (uint32_t i = 0; i < objects.size(); i++)
        {
            transform_data[i] = {
                .scale    = objects[i].getScale(),
                .center   = objects[i].getCenter(),
                .position = objects[i].getPosition(),
                .rotation = objects[i].getRotation()
            };

            colors[i] = objects[i].getColor();
        }

        _quad[1].setData(&transform_data[0], objects.size() * sizeof(Transform));
        _quad[2].setData(&colors[0], objects.size() * sizeof(Color));

        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElementsInstanced(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0, objects.size());
    }
}