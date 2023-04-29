#pragma once

#include <cassert>

namespace cpp2d::Utility
{
    template<typename T>
    class Singleton
    {
    public:
        static T& get();
        static bool destroy();
    };

    template<typename T>
    T& Singleton<T>::get()
    {
        static T* _object = nullptr;

        if (!_object) _object = new T();

        // the singleton shouldn't be accessed after it has been destroyed
        assert(_object);
        return _object;
    }

    template<typename T>
    bool Singleton<T>::destroy()
    {
        T* _object = &Singleton<T>::get();

        // The object shouldn't be destroyed twice
        assert(_object);
        delete _object;
        _object = nullptr;
    }
}