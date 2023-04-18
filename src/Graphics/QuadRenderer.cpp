#include <CPP2D/Graphics.h>

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
        _quad(3)
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

        // Instanced Transform Data
        _quad[1].setDynamic(true);
        _quad[1].setData(nullptr, 0);
        uint32_t sizes[] = { 2, 2, 2, 1 };
        for (uint32_t i = 2; i < 6; i++)
            _quad[1].setAttributeData({ i, sizes[i - 2], true, sizeof(Transform), (i - 2) * (uint32_t)sizeof(Vec2f) });

        _quad[2].setDynamic(true);
        _quad[2].setData(nullptr, 0);
        _quad[2].setAttributeData({ 6, 4, true });
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
        Color color = quad.getColor();

        _quad[1].setData(&transform, sizeof(Transform));
        _quad[2].setData(&color,     sizeof(Color));        

        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElements(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0);

        surface.unbind();
    }

    void QuadRenderer::render(DrawSurface& surface, const Shader& shader) const
    {
        surface.bind();

        shader.bind();
        shader.setUniform("aspectRatio", (float)surface.getSize().x / (float)surface.getSize().y);

        std::vector<Color>     colors(objects->size());
        std::vector<Transform> transform_data(objects->size());
        std::memset(&transform_data[0], 0, sizeof(Transform) * objects->size());
        for (uint32_t i = 0; i < objects->size(); i++)
        {
            transform_data[i] = {
                .scale    = objects->at(i).getScale(),
                .center   = objects->at(i).getCenter(),
                .position = objects->at(i).getPosition(),
                .rotation = objects->at(i).getRotation()
            };

            colors[i] = objects->at(i).getColor();
        }

        _quad[1].setData(&transform_data[0], objects->size() * sizeof(Transform));
        _quad[2].setData(&colors[0], objects->size() * sizeof(Color));

        // Bind surface or whatever
        _quad.bind();
        _quad[0].bind();
        glDrawElementsInstanced(GL_TRIANGLES, _quad.getIndexCount(), GL_UNSIGNED_INT, 0, objects->size());

        surface.unbind();
    }
}