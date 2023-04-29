#pragma once

#include "../Util.h"
#include "../Utility.h"

namespace cpp2d
{
    typedef void* WindowHandle;

    enum class WindowState
    {
        Success,
        NotInitialized,
        glfwInitFailed,
        glfwWindowCreateFailed
    };

    class Window :
        public Utility::State<WindowState>
    {
        WindowHandle _handle;

    public:
        Window(CU32& width, CU32& height, const char* title);
        virtual ~Window();

        bool isOpen() const;

        void close();
        void pollEvents() const;
    };
}