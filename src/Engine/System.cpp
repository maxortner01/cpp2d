#include <cpp2d/Engine.h>

namespace cpp2d
{
    System::System()
    {  _child_systems.reserve(3);  }

    void System::addChild(const System* const child)
    {
        cppAssert(!child->hasChild(this), "Can not add child to a child system!");

        _child_systems.push_back(child);  
    }

    bool System::hasChild(const System* const child) const
    {
        for (uint32_t i = 0; i < _child_systems.size(); i++)
            if (_child_systems[i] == child) 
                return true;
                
        return false;
    }
}