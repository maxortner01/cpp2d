#include <iostream>

#include <vector>

#include <CPP2D/Graphics.h>

int main()
{
    cpp2d::DrawWindow window(640, 360, "hello");

    cpp2d::Quad quad;

    cpp2d::QuadRenderer renderer;

    while (window.isOpen())
    {
        window.pollEvent();
        
        renderer.render(window, quad);

        window.display();
    }

    return 0;
}