#pragma once

#include "NoCopy.h"
#include <cassert>

namespace cpp2d::Utility
{
    /**
     * @class Singleton
     * @brief A template class for creating singleton objects
     *
     * This template class provides a convenient way to create a singleton object of any given type.
     * A singleton object is an object that can only be instantiated once throughout the lifetime of a program.
     *
     * @tparam T The type of object that will be made into a singleton.
     */
    template<typename T>
    class Singleton :
        public Utility::NoCopy
    {
    public:
        /**
         * @brief Returns the singleton instance of the object
         * @return A reference to the singleton instance of the object
         *
         * This method returns a reference to the singleton instance of the object.
         * If the instance does not yet exist, it will be created.
         */
        static T& get();

        /**
         * @brief Destroys the singleton instance of the object
         *
         * This method destroys the singleton instance of the object, if it exists.
         * After the instance has been destroyed, any subsequent calls to get() will create a new instance.
         */
        static void destroy();
    };

    // Implementation
    template<typename T>
    T& Singleton<T>::get()
    {
        static T* _object = nullptr;

        if (!_object) _object = new T();

        // the singleton shouldn't be accessed after it has been destroyed
        assert(_object);
        return *_object;
    }

    template<typename T>
    void Singleton<T>::destroy()
    {
        T* _object = &Singleton<T>::get();

        // The object shouldn't be destroyed twice
        assert(_object);
        delete _object;
        _object = nullptr;
    }
}