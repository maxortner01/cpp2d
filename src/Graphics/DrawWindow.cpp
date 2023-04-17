#include <CPP2D/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    void DrawWindow::clear() const
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}