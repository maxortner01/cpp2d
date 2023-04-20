#include <CPP2D/Utility.h>

#include <GLFW/glfw3.h>

namespace cpp2d
{
namespace Utility
{
    Timer::Timer() :
        _start_time(glfwGetTime())
    {   }

    double Timer::getTime() const
    {
        return glfwGetTime() - _start_time;
    }

    void Timer::restart() 
    {
        _start_time = glfwGetTime();
    }
}
}