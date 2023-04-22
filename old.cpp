#include <iostream>

#include <algorithm>
#include <vector>

#include <CPP2D/Graphics.h>
#include <CPP2D/Systems.h>

using namespace cpp2d;
using namespace cpp2d::Systems;

class BounceAnimation :
    public AnimationInterpolate<Vec2f>
{
public:
    using AnimationInterpolate<Vec2f>::AnimationInterpolate;

    void onStateChanged(const AnimationState& newState) override
    {
        if (getState() == AnimationState::Running && newState == AnimationState::Done)
        {
            setNextAnimation(new BounceAnimation(
                getValue(), 
                getFinalValue(), 
                getStartValue(), 
                2.0, getDelay() + 2.f
            ));
        }
    }
};

class IsometricQuad
{
    const Vec2u _index;
    Quad _quad;

public:
    IsometricQuad(const Vec2u& index, const Vec2u& world_size) :
        _index(index)
    {
        const Vec2i relative_index = Vec2i(
            (int32_t)((float)index.x - (float)world_size.x / 2.f),
            (int32_t)((float)index.y - (float)world_size.y / 2.f)
        );

        _quad.setZ(index.y - (index.x % 2) / 2.0);

        _quad.setScale(Vec2f{ 2.f, 2.f });
        _quad.setPosition(Vec2f({
            relative_index.x,
            relative_index.y - (abs(relative_index.x) % 2) / 2.0
        }));

        const Vec2u tile_count = Vec2u(4, 3);
        const Vec2f tile_size  = Vec2f(
            1.f / (float)tile_count.x, 
            1.f / (float)tile_count.y
        );

        const Vec2u tex_index = Vec2u(0, 0);

        _quad.getScaleRef().y *= 1.5 * 2;
        _quad.setColor({ 1.f, 1.f, 1.f, 1.f });
        _quad.setTextureRect({ 
            tile_size.x * (float)tex_index.x, 
            tile_size.y * (float)(tex_index.y + 1), 
            tile_size.x, 
            tile_size.y * 2
        });
    }

    Vec2u getIndex() const
    {
        return _index;
    }

    Quad& getQuad()
    {
        return _quad;
    }
};


bool compareQuadZ(IsometricQuad q1, IsometricQuad q2)
{
    return (q1.getQuad().getZ() > q2.getQuad().getZ());
}

int main()
{
    DrawWindow window(640 * 2, 360 * 2, "hello");

    ShaderType types[] = {
        ShaderType::Vertex, ShaderType::Fragment
    };
    
    Shader shader(types, 2);
    shader.fromFile(ShaderType::Vertex,   "vertex.glsl"  );
    shader.fromFile(ShaderType::Fragment, "fragment.glsl");
    shader.link();

    Texture texture("res/tiles.png");

    DrawTexture framebuffer(Vec2u(640 * 2, 360 * 2));

    std::vector<IsometricQuad> quads;

    Systems::Animation animation;
    QuadRenderer renderer;
    for (int j = 0; j < 80; j++)
        for (int i = 0; i < 80; i++)
        {
            IsometricQuad quad(Vec2u(i, j), Vec2u(80, 80));
            quads.push_back(quad);
        }

    quads.shrink_to_fit();

    sort(quads.begin(), quads.end(), compareQuadZ);

    std::vector<Quad*> quad_ptrs(quads.size());
    for (int i = 0; i < quads.size(); i++)
    {
        quad_ptrs[i] = &quads[i].getQuad();
        IsometricQuad& quad = quads[i];

        BounceAnimation* anim = new BounceAnimation(
            &quad.getQuad().getCenterRef(), 
            quad.getQuad().getCenter(), 
            quad.getQuad().getCenter() + Vec2f(0, 1), 
            2.0, (float)(quad.getIndex().x + quad.getIndex().y) / 20.f
        );

        animation.pushAnimation(anim);
    }

    Vec2f cam_pos = Vec2f({ 0, 0 });
    float scale = 0.1;

    while (window.isOpen())
    {
        window.pollEvent();

        animation.update();
        
        window.clear();
        framebuffer.clear();
        renderer.begin({});

        // Render the quads
        texture.bind(1);
        framebuffer.getTexture().bind(2);

        shader.setUniform("tex", 1);
        shader.setUniform("cam_scale", scale);
        shader.setUniform("cam_pos", cam_pos);

        renderer.render(framebuffer, shader, &quad_ptrs[0], quads.size());

        // Render the framebuffer
        shader.setUniform("tex", 2);
        shader.setUniform("cam_scale", 1.0f);
        
        Quad bar;
        bar.setTextureRect({ 0, 0, 1, 1 });
        bar.setPosition({ 0.f, 0.f });
        bar.setCenter({ 0.f, 0.f });
        bar.setScale({ (float)window.getSize().x / (float)window.getSize().y * 2.f, 2.f });
        bar.setColor({ 1.f, 1.f, 1.f, 1.f });
        renderer.render(window, shader, bar);

        renderer.end();

        window.setTitle("Hello " + std::to_string(renderer.getFPS()) + " fps, Animations: " + std::to_string(animation.runningAnimations()));
        window.display();
    }

    return 0;
}