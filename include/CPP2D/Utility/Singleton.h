#pragma once

#include <iostream>

namespace cpp2d
{
namespace Utility
{
    template<typename T>
    class Singleton
    {
        inline static T* object = nullptr;

    public:

        void* operator new(size_t size)
        {
            cppAssert(object == nullptr, "");
            object = (T*)std::malloc(size);  
            std::cout << "Allocated " << object << "\n";  
            return object;
        }

        void operator delete(void* ptr)
        {
            std::cout << "deleting " << object << "\n";
            cppAssert(object != nullptr, "");
            ::operator delete(object);
            object = nullptr;
        }

        static T* get()
        {
            return object;
        }

    /*
        static T& get()
        {
            static T* instance = nullptr;

            if (!instance) 
            {
                instance = new T();
            }

            return *instance;
        }*/
    };
}
}