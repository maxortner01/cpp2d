#include <cpp2d/Engine.h>

#include <entt/entt.hpp>

namespace cpp2d
{
    static const char* vertex = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 texcoords;
    layout (location = 2) in vec2 position;
    layout (location = 3) in vec2 scale;
    layout (location = 4) in vec2 center;
    layout (location = 5) in float rotation;
    layout (location = 6) in vec4 color;
    layout (location = 7) in vec4 texrect;

    out vec4 texRect;
    out vec4 vertexColor;
    out vec2 texCoords;

    uniform float aspectRatio;

    void main()
    {
        vec2 pos = ((aPos - center) * scale) + position;

        gl_Position = vec4(pos.x / aspectRatio, pos.y, 0.0, 1.0); 
        vertexColor = color; 
        texCoords   = texcoords;
        texRect     = texrect;
    })";

    static const char* fragment = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec4 texRect;
    in vec4 vertexColor;   
    in vec2 texCoords;

    uniform sampler2D tex;

    void main()
    {
        vec4 tex_color = texture(tex, texCoords);

        FragColor = vertexColor * tex_color;
        if(FragColor.a < 0.1) discard;
    })";
    
    Application::Application(const Vec2u& size, const std::string& title) :
        _window(size.x, size.y, title.c_str()),
        _window_shader(_types, 2)
    {
        _window_shader.fromString(ShaderType::Vertex,   vertex);
        _window_shader.fromString(ShaderType::Fragment, fragment);
        _window_shader.link();
    }

    void Application::run()
    {
        while (_window.isOpen())
        {
            if (!scenes.size()) { _window.close(); continue; }

            // Check if the top scene is done, if so call its destructor and pop
            // it off the top as well as its corresponding surface
            if (scenes[scenes.size() - 1]->getState() == SceneState::Done)
            {
                delete &scenes[scenes.size() - 1];
                scenes.pop_back();
                surfaces.pop_back();
            }
            
            _window.pollEvent();

            // Now we can clear the window and start updating/rendering each of
            // the layers
            _window.clear();

            const float aspectRatio = (float)_window.getSize().x / (float)_window.getSize().y;

            // Now we need to render each layer to the window
            //std::vector<Quad> layers(scenes.size());
            _screen_renderer.begin({});
            for (uint32_t i = 0; i < scenes.size(); i++)
            {
                DrawTexture* surface = surfaces[surfaces.size() - 1];
                
                surface->clear();
                scenes[i]->update(surface);

                Quad quad;
                quad.setPosition(Vec2f(0, 0));
                quad.setCenter(Vec2f(0, 0));
                quad.setTextureRect({ 0, 0, 1, 1 });
                quad.setScale(Vec2f(2.f * aspectRatio, 2));
                quad.setColor({1, 1, 1, 1});

                surface->getTexture().bind(0);
                _window_shader.setUniform("tex", 0);

                _screen_renderer.render(_window, _window_shader, quad);
            }
            _screen_renderer.end();

            _window.display();
        }
    }
}