#pragma once

#include <stack>
#include <string>

#include "../Utility.h"
#include "../Graphics.h"

#include "Scene.h"

namespace cpp2d
{
    class Application
    {
        inline static const ShaderType _types[2] = {
            ShaderType::Vertex, ShaderType::Fragment
        };

        DrawWindow _window;
        Shader     _window_shader;

        QuadRenderer _screen_renderer;
        std::vector<DrawTexture*> surfaces;
        std::vector<Scene*> scenes;

    public:
        Application(const Vec2u& dimensions, const std::string& title);

        template<typename T, typename... Args>
        void pushScene(Args... args)
        {
            T* new_scene = new T(args...);

            scenes.push_back(new_scene);
            surfaces.push_back(&new_scene->getDrawTexture());
            scenes.shrink_to_fit();
            surfaces.shrink_to_fit();
        }

        void run();
    };
}