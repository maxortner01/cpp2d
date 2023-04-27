#include <cpp2d/Graphics.h>

#include <iostream>

namespace cpp2d
{
    //--------------- PRIVATE ---------------------

    //--------------- PUBLIC ---------------------

    GraphicsInstance::GraphicsInstance() :
        _init(false)
    {   }

    void GraphicsInstance::init() 
    {
        // Init gl3w
        if (isInitialized()) return;

        // I hate this, but segfaults vertex arrays on mac otherwise...
        glewExperimental = GL_TRUE; 
        if (glewInit() != GLEW_OK)
        {
            setState(GraphicsState::gl3wFailed);
            return;
        }

        setState(GraphicsState::Success);
        _init = true;
    }

    bool GraphicsInstance::isInitialized() const
    {
        return _init;
    }

    void GraphicsInstance::drawInstanced(const uint32_t& indexCount, const uint32_t& instanceCount) const
    {
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
    }

    void GraphicsInstance::setViewport(const UnsignedRect& rect) const
    {
        glViewport(rect.x, rect.y, rect.width, rect.height);
    }
}