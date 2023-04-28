#pragma once

#include "../util.h"
#include "../Utility.h"
#include "Sizable.h"

#include <vector>

namespace cpp2d
{
namespace Graphics
{
    class GDI;
}

    enum class WindowState
    {
        Success,
        Destroyed,
        glfwInitFailed,
        glfwWindowCreateFailed
    };

    class Window : 
        public Utility::State<WindowState>,
        UnsignedSizable
    {
    protected:
        void* _window;

#   ifdef GDI_VULKAN
        gdiSurface   _surface;
        gdiSwapChain _swap_chain;
        gdiFormat    _image_format;
        // Not sure how I feel about vector being used here,
        // but vkBoostrap uses it for some reason... 

        // Need to reimplement without vectors... they suck
        std::vector<gdiImage>     _images;
        std::vector<gdiImageView> _image_views;
#   endif

    public:
        friend class Graphics::GDI;

        Window(const Vec2u& size, const char* title);
        Window(const U32& width, const U32& height, const char* title);
        virtual ~Window();

        using UnsignedSizable::getSize;

        void pollEvent();
        void display();

        void setTitle(const std::string& title) const;

        void close() const;
        bool isOpen() const;
    };
}