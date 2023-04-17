#include <iostream>

#include <vector>

#include <CPP2D/Graphics.h>

using namespace cpp2d;

int main()
{
    DrawWindow window(640 * 2, 360 * 2, "hello");

    ShaderType types[] = {
        ShaderType::Vertex, ShaderType::Fragment
    };
    
    Shader shader(&types[0], 2);
    shader.fromFile(ShaderType::Vertex,   "vertex.glsl"  );
    shader.fromFile(ShaderType::Fragment, "fragment.glsl");
    shader.link();

    QuadRenderer renderer;
    for (int j = 0; j < 100; j++)
        for (int i = 0; i < 100; i++)
        {
            Quad& quad = renderer.emplaceObject();
            quad.setPosition(Vec2f(-1 + (float)i / 50.f, -1 + (float)j / 50.f));
            quad.setScale(Vec2f(0.01, 0.01));
            quad.setColor({ (float)i / 100.f, (float)j / 100.f, 0, 1.f });
        }

    while (window.isOpen())
    {
        window.pollEvent();

        window.clear();
        renderer.render(window, shader);

        window.display();
    }

    return 0;
}