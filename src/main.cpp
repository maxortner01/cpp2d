#include <CPP2D/Graphics.h>
#include <CPP2D/Systems.h>

using namespace cpp2d;

class TestScene :
    public Scene
{
    const ShaderType _type[2] = {
        ShaderType::Vertex, ShaderType::Fragment
    };

    QuadRenderer quads;
    Shader shader;

public:
    TestScene(const Vec2u& size) :
        Scene(size),
        shader(_type, 2)
    {
        shader.fromFile(ShaderType::Vertex,   "vertex.glsl");
        shader.fromFile(ShaderType::Fragment, "fragment.glsl");
        shader.link();
    }

    void update(DrawSurface* surface) override
    {
        Quad quad;

        quad.setScale({ 0.5, 0.5 });
        quad.setColor({ 1, 1, 1, 1});

        shader.setUniform("cam_pos", Vec2f(0, 0));
        shader.setUniform("cam_scale", 1.f);

        quads.render(*surface, shader, quad);
    }
};

int main()
{
    Application app({ 1280, 720 }, "Application");

    app.pushScene<TestScene>(Vec2u(1280, 720));

    app.run();
}