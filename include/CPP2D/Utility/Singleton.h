#pragma once

namespace cpp2d
{
namespace Utility
{
    template<typename T>
    class Singleton
    {
    public:
        static T& get()
        {
            static T* instance = nullptr;

            if (!instance) 
            {
                instance = new T();
            }

            return *instance;
        }
    };
}
}