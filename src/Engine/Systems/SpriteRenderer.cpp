#include <cpp2d/Engine.h>

#include <entt/entt.hpp>
#include <GL/glew.h>

namespace cpp2d
{
namespace Systems
{

#pragma region SHADER_CODE

    static const char* vertex = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 texcoords;
    layout (location = 2) in vec2 position;
    layout (location = 3) in vec2 scale;
    layout (location = 4) in float rotation;
    layout (location = 5) in vec4 color;
    layout (location = 6) in uint texid;
    layout (location = 7) in vec4 rect;

    out vec4 texRect;
    out vec2 texCoords;
    out vec4 vertexColor;
    flat out uint out_texid;

    struct Camera
    {
        vec2  position;
        float scale;
    };

    uniform Camera camera;
    uniform float  aspectRatio;

    mat2 ROTATION_MATRIX = mat2(
        cos(rotation), -sin(rotation),
        sin(rotation), cos(rotation)
    );

    void main()
    {
        vec2 pos = ( ROTATION_MATRIX * (aPos * scale) + position ) * camera.scale - camera.position;

        gl_Position = vec4(pos.x / aspectRatio, pos.y, 0.0, 1); 
        vertexColor = color; 
        out_texid   = texid;
        texCoords   = texcoords;
        texRect     = rect;
    })";

    static const char* fragment = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec4 texRect;
    in vec2 texCoords;
    in vec4 vertexColor;   
    flat in uint out_texid;

    uniform int texture_ids[5];
    uniform sampler2D textures[5];

    void main()
    {
        vec4 tex_color = texture(textures[out_texid], texCoords * texRect.zw + texRect.xy);

        FragColor = vertexColor * (texture_ids[out_texid] > 0?tex_color:vec4(1));
        if(FragColor.a < 0.1) discard;
    })";

#pragma endregion SHADER_CODE

#pragma region SPRITE_VERTEX_ARRAY

    SpriteVertexArray::SpriteVertexArray(const uint32_t& extraBuffers) :
        VertexArray(1 + extraBuffers)
    {
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

        setIndices(&indices[0], 6);
        at(0).setDynamic(false);
        at(0).setData(vertices, sizeof(Vertex) * 4);
        at(0).setAttributeData({ 0, 2, false, sizeof(Vertex), 0 });
        at(0).setAttributeData({ 1, 2, false, sizeof(Vertex), sizeof(Vec2f) });
    }

#pragma endregion SPRITE_VERTEX_ARRAY

#pragma region SPRITE_SHADER

    SpriteShader::SpriteShader() :
        Shader({ ShaderType::Vertex, ShaderType::Fragment })
    {
        fromString(ShaderType::Vertex, vertex);
        fromString(ShaderType::Fragment, fragment);
        link();
    }

#pragma endregion SPRITE_SHADER

#pragma region DYNAMIC_SPRITE_RENDERER

    void SpriteRenderer::submitRender(Sprite* const sprites, const size_t count)
    {
        SpriteShader::get().bind();
        
        /*
        quad[1].setData(transforms, sizeof(Transform) * count);
        quad[2].setData(colors, sizeof(Color) * count);
        quad[3].setData(textures, sizeof(uint32_t) * count);
        quad[4].setData(rects, sizeof(FloatRect) * count);*/
        quad[1].setData(sprites, sizeof(Sprite) * count);

        quad.bind();
        quad[0].bind();
        GraphicsInstance::get().drawInstanced(quad.getIndexCount(), count);

        SpriteShader::get().unbind();
    }

    SpriteRenderer::SpriteRenderer() :
        quad(1)
    {
        // Transform data structure
        quad[1].setDynamic(true);
        quad[1].setData(nullptr, 0);
        quad[1].setAttributeData({ 2, 2, true, sizeof(Sprite), offsetof(struct Sprite, transform) + offsetof(struct Transform, position)}); // position
        quad[1].setAttributeData({ 3, 2, true, sizeof(Sprite), offsetof(struct Sprite, transform) + offsetof(struct Transform, scale)}); // scale
        quad[1].setAttributeData({ 4, 1, true, sizeof(Sprite), offsetof(struct Sprite, transform) + offsetof(struct Transform, rotation)}); // rotation
        quad[1].setAttributeData({ 5, 4, true, sizeof(Sprite), offsetof(struct Sprite, color)}); // color
        quad[1].setAttributeData({ 6, 1, true, sizeof(Sprite), offsetof(struct Sprite, texture), DataType::UnsignedInt}); // texture index
        quad[1].setAttributeData({ 7, 4, true, sizeof(Sprite), offsetof(struct Sprite, rect) });
    }

    SpriteRenderer::SceneData& SpriteRenderer::getSceneData()
    {
        return _scene_data;
    }

    void SpriteRenderer::setTextureList(Texture* textureList, const uint32_t& count)
    {
        _scene_data.textureList  = textureList;
        _scene_data.textureCount = count;
    }

    // The best I could do right now without totally recapturing the Sprite component pointers
    // on the heap every frame. The size_hint seems to be pretty close
    void SpriteRenderer::update(Scene* const scene, double dt) 
    {
        std::cout << "running sprite renderer\n";

        auto view = scene->view<Sprite>(entt::exclude<Static>);

        std::vector<Sprite> sprites;
        sprites.reserve(view.size_hint());

        view.each([ &sprites ](auto entity, Sprite& sprite) {
            sprites.push_back(sprite);
        });

        // possibly use reactive callbacks to perform this sort only when the position
        // of sprites change
        std::sort(sprites.begin(), sprites.end(), [](Sprite& a, Sprite& b) { return a.z > b.z; });
        
        if (!sprites.size()) return;

        const float aspectRatio = (float)scene->getSize().x / (float)scene->getSize().y;
        // Not sure if/why uniform needs to be bound...
        SpriteShader::get().bind();
        SpriteShader::get().setUniform("aspectRatio", aspectRatio);
        SpriteShader::get().setUniform("camera.position", _scene_data.camera.position);
        SpriteShader::get().setUniform("camera.scale",    _scene_data.camera.scale);

        // Bind texture info
        scene->getDrawTexture().bind();
        for (int i = 0; i < _scene_data.textureCount; i++)
        {
            _scene_data.textureList[i].bind(i);
            SpriteShader::get().setUniform("textures[" + std::to_string(i) + "]", i);
            SpriteShader::get().setUniform("texture_ids[" + std::to_string(i) + "]", (int)_scene_data.textureList[i].getID());
        }

        submitRender(&sprites[0], sprites.size());
        scene->getDrawTexture().unbind();
    }

#pragma endregion DYNAMIC_SPRITE_RENDERER

}
}