#include <cpp2d/Engine.h>

#include <entt/entt.hpp>
#include <GL/glew.h>

namespace cpp2d
{
namespace Systems
{
    static const char* vertex = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 position;
    layout (location = 2) in vec2 scale;
    layout (location = 3) in float rotation;
    layout (location = 4) in vec4 color;

    out vec4 vertexColor;

    uniform float aspectRatio;

    mat2 ROTATION_MATRIX = mat2(
        cos(rotation), -sin(rotation),
        sin(rotation), cos(rotation)
    );

    void main()
    {
        vec2 pos = ( ROTATION_MATRIX * (aPos * scale) + position );

        gl_Position = vec4(pos.x / aspectRatio, pos.y, 0.0, 1.0); 
        vertexColor = color; 
    })";

    static const char* fragment = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec4 vertexColor;   

    //uniform sampler2D tex;

    void main()
    {
        //vec4 tex_color = texture(tex, texCoords);

        FragColor = vertexColor; //* tex_color;
        FragColor = vec4(1, 0, 0, 1);
        if(FragColor.a < 0.1) discard;
    })";

    void SpriteRenderer::submitRender(std::vector<Transform>* transforms, std::vector<Color>* colors)
    {
        spriteShader.bind();
        
        quad[1].setData(&(*transforms->begin()), sizeof(Transform) * transforms->size());
        quad[2].setData(&(*colors->begin()), sizeof(Color) * colors->size());

        quad.bind();
        quad[0].bind();
        GraphicsInstance::get().drawInstanced(quad.getIndexCount(), colors->size());

        spriteShader.unbind();
    }

    SpriteRenderer::SpriteRenderer() :
        quad(3),
        spriteShader({ ShaderType::Vertex, ShaderType::Fragment })
    {
        // Construct the sprite shader
        spriteShader.fromString(ShaderType::Vertex,   vertex);
        spriteShader.fromString(ShaderType::Fragment, fragment);
        spriteShader.link();

        // Construct the quad vertex array
        const Vec2f vertices[] = {
            Vec2f(-1,  1),
            Vec2f( 1,  1),
            Vec2f( 1, -1),
            Vec2f(-1, -1)
        };

        const uint32_t indices[] = {
            0, 1, 2, 2, 3, 0
        };

        quad.setIndices(&indices[0], 6);
        quad[0].setDynamic(false);
        quad[0].setData(vertices, sizeof(Vec2f) * 4);
        quad[0].setAttributeData({ 0, 2 });

        // Transform data structure
        quad[1].setDynamic(true);
        quad[1].setData(nullptr, 0);
        quad[1].setAttributeData({1, 2, true, sizeof(Transform), 0 * sizeof(Vec2f)}); // position
        quad[1].setAttributeData({2, 2, true, sizeof(Transform), 1 * sizeof(Vec2f)}); // scale
        quad[1].setAttributeData({3, 1, true, sizeof(Transform), 2 * sizeof(Vec2f)}); // rotation

        // Color data structure
        quad[2].setDynamic(true);
        quad[2].setData(nullptr, 0);
        quad[2].setAttributeData({ 4, 4, true });
    }

    void SpriteRenderer::update(Scene* const scene, double dt) 
    {
        scene->sort<Sprite>([](const auto& lhs, const auto& rhs) {
            return lhs.texture < rhs.texture;
        });

        auto view = scene->view<Sprite>();
        if (!view.size()) return;

        std::vector<Sprite*> sprites(view.size());
        uint32_t enumerator = 0;
        for (auto entity : view) sprites[enumerator++] = &view.get<Sprite>(entity);

        std::vector<Color> colors;
        std::vector<Systems::Transform> transforms;

        const float aspectRatio = (float)scene->getSize().x / (float)scene->getSize().y;
        // Not sure if uniform needs to be bound...
        spriteShader.bind();
        spriteShader.setUniform("aspectRatio", aspectRatio);

        enumerator = 0;
        uint32_t last_index = 0;
        uint32_t current_texture = sprites[0]->texture;
        for (auto& sprite : sprites)
        {
            if (sprite->texture != current_texture)
            {
                scene->getDrawTexture().bind();
                Texture::bindTexture(current_texture, 0);

                submitRender(&transforms, &colors);
                scene->getDrawTexture().unbind();

                // submit from begin() + last_index to begin() + enumerator for rendering
                last_index = enumerator;
                current_texture = sprites[enumerator]->texture;

                // Empty the buffers
                transforms.clear();
                colors.clear();
            }

            colors.push_back(sprite->color);
            transforms.push_back(sprite->transform);

            enumerator++;
        }

        if (transforms.size())
        {
            scene->getDrawTexture().bind();
            Texture::bindTexture(current_texture, 0);

            submitRender(&transforms, &colors);
            scene->getDrawTexture().unbind();
        }
    }
}
}