#pragma once

#include "../util.h"
#include "../Utility.h"
#include "Sizable.h"

namespace cpp2d
{
    enum class WindowState
    {
        Success,
        glfwInitFailed,
        glfwWindowCreateFailed
    };

    class Window : 
        public Utility::State<WindowState>,
        UnsignedSizable
    {
    protected:
        void* _window;

    public:
        Window(const uint32_t& width, const uint32_t& height, const char* title);
        ~Window();

        using UnsignedSizable::getSize;

        void pollEvent();
        void display();

        bool isOpen() const;
    };
}