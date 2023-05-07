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

    class CPP2D_DLL Window :
        public Utility::State<WindowState>,
        public Utility::NoCopy
    {
        WindowHandle _handle;

    public:
        Window(CU32& width, CU32& height, const char* title);
        virtual ~Window();

        bool isOpen() const;

        void setTitle(const char* title) const;

        void close();
        void pollEvents() const;

        WindowHandle getHandle() const;
    };
}