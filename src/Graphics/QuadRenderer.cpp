#include <CPP2D/Graphics.h>

#include <iostream>
#include <GL/glew.h>

namespace cpp2d
{
    struct Transform {
        Vec2f position, scale, center;
        float rotation;
    };

    struct Vertex {
        Vec2f position, texcoord;
    };
    
    QuadRenderer::QuadRenderer() :
        _quad(4)
    {
        const Vertex vertices[] = {
            {{ -0.5,  0.5 }, { 0, 1 }},
            {{  0.5,  0.5 }, { 1, 1 }},
            {{  0.5, -0.5 }, { 1, 0 }},
            {{ -0.5, -0.5 }, { 0, 0 }}
        };

        const uint32_t indices[] = {
            0, 1, 2, 2, 3, 0
        };
        
        // Instance vertex data
        _quad[0].setData(&vertices[0], sizeof(Vertex) * 4);
        _quad[0].setAttributeData({ 0, 2, false, sizeof(Vertex), 0 });
        _quad[0].setAttributeData({ 1, 2, false, sizeof(Vertex), sizeof(Vec2f) });
        _quad.setIndices(&indices[0], 6);

        // ------------- Instanced Data -------------

        // The Transform data
        _quad[1].setDynamic(true);
        _quad[1].setData(nullptr, 0);
        uint32_t sizes[] = { 2, 2, 2, 1 };
        for (uint32_t i = 2; i < 6; i++)
            _quad[1].setAttributeData({ i, sizes[i - 2], true, sizeof(Transform), (i - 2) * (uint32_t)sizeof(Vec2f) });

        // Color
        _quad[2].setDynamic(true);
        _quad[2].setData(nullptr, 0);
        _quad[2].setAttributeData({ 6, 4, true });

        // Texture rect
        _quad[3].setDynamic(true);
        _quad[3].setData(nullptr, 0);
        _quad[3].setAttributeData({ 7, 4, true });
    }

    void QuadRenderer::render(DrawSurface& surface, const Shader& shader, const Quad& quad) const
    {
        surface.bind();

        shader.bind();
        shader.setUniform("aspectRatio", (float)surface.getSize().x / (float)surface.getSize().y);

        Transform transform = {
            .scale    = quad.getScale(),
            .center   = quad.getCenter(),
            .position = quad.getPosition(),
            .rotation = quad.getRotation()
        };
        Color     color = quad.getColor();
        FloatRect rect  = quad.getTextureRect();

        _quad[1].setData(&transform, sizeof(Transform));
        _quad[2].setData(&color,     sizeof(Color));    
        _quad[3].setData(&rect,      sizeof(FloatRect));    

        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElements(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0);

        surface.unbind();
    }

    void QuadRenderer::render(DrawSurface& surface, const Shader& shader, const Quad* const objects, const uint32_t& count) const
    {
        surface.bind();

        shader.bind();
        shader.setUniform("aspectRatio", (float)surface.getSize().x / (float)surface.getSize().y);

        std::vector<FloatRect> rects(count);
        std::vector<Color>     colors(count);
        std::vector<Transform> transform_data(count);
        std::memset(&transform_data[0], 0, sizeof(Transform) * count);
        for (uint32_t i = 0; i < count; i++)
        {
            transform_data[i] = {
                .scale    = objects[i].getScale(),
                .center   = objects[i].getCenter(),
                .position = objects[i].getPosition(),
                .rotation = objects[i].getRotation()
            };

            colors[i] = objects[i].getColor();
            rects[i]  = objects[i].getTextureRect();
        }

        _quad[1].setData(&transform_data[0], count * sizeof(Transform));
        _quad[2].setData(&colors[0], count * sizeof(Color));
        _quad[3].setData(&rects[0],  count * sizeof(FloatRect));

        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElementsInstanced(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0, count);

        surface.unbind();
    }

    void QuadRenderer::render(DrawSurface& surface, const Shader& shader, const Quad* const* const objects, const uint32_t& count) const
    {
        surface.bind();

        shader.bind();
        shader.setUniform("aspectRatio", (float)surface.getSize().x / (float)surface.getSize().y);

        std::vector<FloatRect> rects(count);
        std::vector<Color>     colors(count);
        std::vector<Transform> transform_data(count);
        std::memset(&transform_data[0], 0, sizeof(Transform) * count);
        for (uint32_t i = 0; i < count; i++)
        {
            transform_data[i] = {
                .scale    = objects[i]->getScale(),
                .center   = objects[i]->getCenter(),
                .position = objects[i]->getPosition(),
                .rotation = objects[i]->getRotation()
            };

            colors[i] = objects[i]->getColor();
            rects[i]  = objects[i]->getTextureRect();
        }

        _quad[1].setData(&transform_data[0], count * sizeof(Transform));
        _quad[2].setData(&colors[0], count * sizeof(Color));
        _quad[3].setData(&rects[0],  count * sizeof(FloatRect));

        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElementsInstanced(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0, count);

        surface.unbind();
    }
}