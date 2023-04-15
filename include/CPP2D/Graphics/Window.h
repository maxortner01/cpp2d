#pragma once

#include "../util.h"
#include "../Utility.h"

namespace cpp2d
{
    enum class WindowState
    {
        Success,
        glfwInitFailed,
        glfwWindowCreateFailed
    };

    class Window : 
        public Utility::State<WindowState>
    {
        void* _window;

    public:
        Window(const uint32_t& width, const uint32_t& height, const char* title);
        ~Window();

        void pollEvent();
        void display();

        bool isOpen() const;
    };
}