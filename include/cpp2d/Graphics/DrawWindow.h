#pragma once

namespace cpp2d
{
    class Window;

    class DrawWindow :
        public Window
    {
    public:
        DrawWindow(CU32& width, CU32& height, const char* title);
        ~DrawWindow();
    };
}