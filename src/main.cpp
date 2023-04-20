#include <iostream>

#include <algorithm>
#include <vector>

#include <CPP2D/Graphics.h>
#include <CPP2D/Systems.h>

using namespace cpp2d;

bool compareQuadZ(Quad q1, Quad q2)
{
    return (q1.getZ() < q2.getZ());
}

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

    Texture texture("res/tiles.png");

    DrawTexture framebuffer(Vec2u(640 * 2, 360 * 2));

    std::vector<Quad> quads(13 * 6);

    Systems::Animation animation;

    uint32_t counter = 0;
    QuadRenderer renderer;
    for (int j = -8; j < 18 && counter < quads.size(); j++)
        for (int i = 0; i < 12 && counter < quads.size(); i++)
        {
            Quad& quad = quads[counter];

            quad.setZ(i);

            quad.setPosition(Vec2f({
                (float)(2 * j + i) / 2.0,
                - (float)i/4.0
            }));

            quad.setColor({ 1.f, 1.f, 1.f, 1.f });
            quad.setTextureRect({ 0, 0.5f, 0.5f, 0.5f });

            //quad.setCenter(Vec2f({ 7.f, -4.f }));
            if (j + i / 2.0 < 0 || j + i / 2.0 > 6) continue;

            animation.emplaceAnimation<Vec2f>(
                &quad.getCenterRef(), 
                Vec2f({7.f, 1.f}), 
                Vec2f({7.f, -4.f}), 
                2.0, (double)(quad.getPosition().x + quad.getPosition().y) / 2.0
            );


            counter++;
        }

    sort(quads.begin(), quads.end(), compareQuadZ);

    renderer.submitObjects(&quads);

    const uint32_t frame_count = 20;
    int frames[frame_count] = { 0 };

    Vec2f cam_pos = Vec2f({ 0, 0 });
    float scale = 0.2;

    uint32_t index = 0;
    Utility::Timer timer;
    while (window.isOpen())
    {
        Utility::Timer frame_time;

        /*
        for (int i = 0; i < quads.size(); i++)
        {
            Quad& quad = quads[i];

            float mod = 1 - cos(0.5 * (quad.getPosition().x + quad.getPosition().y) + 0.5* timer.getTime());
            quad.setCenter(Vec2f({ 7.f, -4.f + 0.5 * pow(mod, 4) }));
        }*/

        //std::cout << animation.runningAnimations() << "\n";

        animation.update();

        window.pollEvent();

        texture.bind(1);
        shader.bind();
        shader.setUniform("tex", 1);
        shader.setUniform("cam_scale", scale);
        shader.setUniform("cam_pos", cam_pos);

        window.clear();
        framebuffer.clear();
        renderer.render(framebuffer, shader);

        texture.unbind();

        framebuffer.getTexture().bind(1);
        shader.bind();
        shader.setUniform("tex", 1);

        shader.setUniform("cam_scale", 1.0f);
        Quad bar;
        bar.setTextureRect({ 0, 0, 1, 1 });
        bar.setPosition({ 0.f, 0.f });
        bar.setCenter({ 0.f, 0.f });
        bar.setScale({ (float)window.getSize().x / (float)window.getSize().y * 2.f, 2.f });
        bar.setColor({ 1.f, 1.f, 1.f, 1.f });
        renderer.render(window, shader, bar);

        int   frame_sum = 0;
        for (int i = 0; i < frame_count; i++)
            frame_sum += frames[i];

        const float average_frame_time = (float)frame_sum / (float)frame_count;
        window.setTitle("Hello " + std::to_string((int)average_frame_time) + " fps, Animations: " + std::to_string(animation.runningAnimations()));

        frames[(index++) % frame_count] = (int)(1.f / frame_time.getTime());

        window.display();
    }

    return 0;
}