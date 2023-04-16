#include <CPP2D/Graphics.h>

#include <iostream>
#include <GL/glew.h>

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
}