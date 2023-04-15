#include <CPP2D/Graphics.h>

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
    }

    bool GraphicsInstance::isInitialized() const
    {
        return _init;
    }
}