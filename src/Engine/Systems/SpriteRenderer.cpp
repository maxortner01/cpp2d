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
    layout (location = 1) in vec2 texcoords;
    layout (location = 2) in vec2 position;
    layout (location = 3) in vec2 scale;
    layout (location = 4) in float rotation;
    layout (location = 5) in vec4 color;
    layout (location = 6) in uint texid;

    out vec2 texCoords;
    out vec4 vertexColor;
    flat out uint out_texid;

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
        out_texid   = texid;
        texCoords   = texcoords;
    })";

    static const char* fragment = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec2 texCoords;
    in vec4 vertexColor;   
    flat in uint out_texid;

    uniform int texture_ids[5];
    uniform sampler2D textures[5];

    void main()
    {
        vec4 tex_color = texture(textures[out_texid], texCoords);

        FragColor = vertexColor * (texture_ids[out_texid] > 0?tex_color:vec4(1));
        if(FragColor.a < 0.1) discard;
    })";

    void SpriteRenderer::submitRender(Transform* transforms, Color* colors, uint32_t* textures, size_t count)
    {
        spriteShader.bind();
        
        quad[1].setData(transforms, sizeof(Transform) * count);
        quad[2].setData(colors, sizeof(Color) * count);
        quad[3].setData(textures, sizeof(uint32_t) * count);

        quad.bind();
        quad[0].bind();
        GraphicsInstance::get().drawInstanced(quad.getIndexCount(), count);

        spriteShader.unbind();
    }

    SpriteRenderer::SpriteRenderer() :
        quad(4),
        spriteShader({ ShaderType::Vertex, ShaderType::Fragment })
    {
        // Construct the sprite shader
        spriteShader.fromString(ShaderType::Vertex,   vertex);
        spriteShader.fromString(ShaderType::Fragment, fragment);
        spriteShader.link();

        // Construct the quad vertex array
        struct Vertex
        {
            Vec2f position;
            Vec2f texcoord;
        };

        const Vertex vertices[] = {
            { { -1,  1 }, { 0, 1 } },
            { {  1,  1 }, { 1, 1 } },
            { {  1, -1 }, { 1, 0 } },
            { { -1, -1 }, { 0, 0 } }
        };

        const uint32_t indices[] = {
            0, 1, 2, 2, 3, 0
        };

        quad.setIndices(&indices[0], 6);
        quad[0].setDynamic(false);
        quad[0].setData(vertices, sizeof(Vertex) * 4);
        quad[0].setAttributeData({ 0, 2, false, sizeof(Vertex), 0 });
        quad[0].setAttributeData({ 1, 2, false, sizeof(Vertex), sizeof(Vec2f) });

        // Transform data structure
        quad[1].setDynamic(true);
        quad[1].setData(nullptr, 0);
        quad[1].setAttributeData({2, 2, true, sizeof(Transform), 0 * sizeof(Vec2f)}); // position
        quad[1].setAttributeData({3, 2, true, sizeof(Transform), 1 * sizeof(Vec2f)}); // scale
        quad[1].setAttributeData({4, 1, true, sizeof(Transform), 2 * sizeof(Vec2f)}); // rotation

        // Color data structure
        quad[2].setDynamic(true);
        quad[2].setData(nullptr, 0);
        quad[2].setAttributeData({ 5, 4, true });

        quad[3].setDynamic(true);
        quad[3].setData(nullptr, 0);
        quad[3].setAttributeData({ 6, 1, true, 0, 0, DataType::UnsignedInt }); // texture ids
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

        std::vector<uint32_t> textures;
        std::vector<Color> colors;
        std::vector<Systems::Transform> transforms;

        textures.reserve(view.size());
        colors.reserve(view.size());
        transforms.reserve(view.size());

        const float aspectRatio = (float)scene->getSize().x / (float)scene->getSize().y;
        // Not sure if/why uniform needs to be bound...
        spriteShader.bind();
        spriteShader.setUniform("aspectRatio", aspectRatio);
        
        for (auto& sprite : sprites)
        {
            textures.push_back(sprite->texture);
            colors.push_back(sprite->color);
            transforms.push_back(sprite->transform);
        }

        // Get the unique texture ids
        std::vector<uint32_t> tex_ids = textures;
        std::vector<uint32_t>::iterator it = std::unique(tex_ids.begin(), tex_ids.end());
        tex_ids.resize(std::distance(tex_ids.begin(), it));

        for (uint32_t& texture : textures)
            texture = (uint32_t)std::distance(tex_ids.begin(), std::find(tex_ids.begin(), tex_ids.end(), texture));


        if (transforms.size())
        {
            scene->getDrawTexture().bind();
            for (int i = 0; i < tex_ids.size(); i++)
            {
                Texture::bindTexture(tex_ids[i], i);
                spriteShader.setUniform("textures[" + std::to_string(i) + "]", i);
                spriteShader.setUniform("texture_ids[" + std::to_string(i) + "]", (int)tex_ids[i]);
            }

            //spriteShader.setUniform("textures", &tex_ids[0], tex_ids.size());

            submitRender(&transforms[0], &colors[0], &textures[0], view.size());
            scene->getDrawTexture().unbind();
        }
    }
}
}