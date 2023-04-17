#include <iostream>

#include <vector>

#include <CPP2D/Graphics.h>

using namespace cpp2d;

int main()
{
    DrawWindow window(640, 360, "hello");

    ShaderType types[] = {
        ShaderType::Vertex, ShaderType::Fragment
    };

    
    Shader shader(&types[0], 2);
    shader.fromFile(ShaderType::Vertex,   "vertex.glsl"  );
    shader.fromFile(ShaderType::Fragment, "fragment.glsl");
    shader.link();

    QuadRenderer renderer;
    Quad& quad = renderer.emplaceObject();

    while (window.isOpen())
    {
        window.pollEvent();

        window.clear();
        shader.bind();
        renderer.render(window);

        window.display();
    }

    return 0;
}