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

    Texture texture("texture.png");

    DrawTexture framebuffer(Vec2u(640, 360));

    std::vector<Quad> quads(100 * 100);

    uint32_t counter = 0;
    QuadRenderer renderer;
    for (int j = 0; j < 100; j++)
        for (int i = 0; i < 100; i++)
        {
            Quad& quad = quads[counter++];
            quad.setPosition(Vec2f(-5 + (float)i / 50.f * 5.f, -5 + (float)j / 50.f * 5.f));
            quad.setScale(Vec2f(0.01 * 5.f, 0.01 * 5.f));
            quad.setColor({ (float)i / 100.f, (float)j / 100.f, 0, 1.f });
        }

    renderer.submitObjects(&quads);

    uint32_t index = 0;
    while (window.isOpen())
    {
        window.pollEvent();

        for (int i = 0; i < quads.size(); i++)
        {
            quads[i].setCenter(
                Vec2f(
                    cos(2.f * quads[i].getPosition().x + (float)index * 0.01), 
                    sin(2.f * quads[i].getPosition().y + (float)index * 0.01)
                )
            );
        }

        texture.bind(1);
        shader.bind();
        shader.setUniform("tex", 1);

        window.clear();
        framebuffer.clear();
        renderer.render(framebuffer, shader);

        texture.unbind();

        Quad bar;
        bar.setColor({ 0, 0, 0, 1.f });
        bar.setScale({ 4.f, 0.6f });
        bar.setCenter({ 0.f, 0.6f / 2.f });
        bar.setPosition({ 0.f, 1.f });
        renderer.render(framebuffer, shader, bar);

        bar.setCenter({ 0.f, -0.6f / 2.f });
        bar.setPosition({ 0.f, -1.f });
        renderer.render(framebuffer, shader, bar);

        framebuffer.getTexture().bind(1);
        shader.bind();
        shader.setUniform("tex", 1);

        bar.setPosition({ 0.f, 0.f });
        bar.setCenter({ 0.f, 0.f });
        bar.setScale({ (float)window.getSize().x / (float)window.getSize().y * 2.f, 2.f });
        //bar.setScale({ 1.f, 1.f });
        bar.setColor({ 1.f, 1.f, 1.f, 1.f });
        renderer.render(window, shader, bar);


        window.display();

        index++;
    }

    return 0;
}