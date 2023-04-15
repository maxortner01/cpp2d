#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <CPP2D/Graphics.h>

int main()
{
    cpp2d::Window window(640, 360, "hello");

    while (window.isOpen())
    {
        window.pollEvent();

        window.display();
    }

    return 0;
}