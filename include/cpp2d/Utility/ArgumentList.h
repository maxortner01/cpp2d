#pragma once

#include "../Util.h"
#include <cassert>
#include <iostream>

namespace cpp2d::Utility
{
    /**
     * @class ArgumentList
     * @brief A class that provides a variable-length argument list for function calls
     *
     * This class provides a way to create a list of arguments with a variable number
     * of types and pass them to a function. The argument list is stored as a block of
     * memory and can be accessed using the set() and get() methods.
     */
    class ArgumentList
    {
    public:
        /**
         * @brief Constructs a new ArgumentList object with empty memory.
         */
        ArgumentList();

        /**
         * @brief Frees the allocated memory.
         */
        void free();

        /**
         * @brief Allocates memory for the specified types.
         * 
         * @tparam _Types The types to allocate memory for.
         * @param types The values to allocate memory for.
         */
        template<typename... _Types>
        void allocate(const _Types&... types);

        /**
         * @brief Sets the value of the argument at the current write position.
         * 
         * @tparam T The type of the argument.
         * @param value The value to set.
         */
        template<typename T>
        void set(const T& value);

        /**
         * @brief Sets the values of the arguments starting from the current write position.
         * 
         * @tparam _First The type of the first argument.
         * @tparam _Types The types of the other arguments.
         * @param first The value of the first argument.
         * @param types The values of the other arguments.
         */
        template<typename _First, typename... _Types>
        void set(const _First& first, const _Types&... types);

        /**
         * @brief Retrieves the value of the argument at the current read position.
         * 
         * @tparam T The type of the argument.
         * @return T The value of the argument.
         */
        template<typename T>
        T get();

        /**
         * @brief Gets a pointer to the allocated memory.
         * 
         * @return void* A pointer to the allocated memory.
         */
        void* ptr() const;

    private:
        U32   _bytes;
        void* _ptr;

        char* _read_it;
        char* _write_it;

        template<typename _Last>
        U32 _count_bytes(const _Last& last);

        template<typename _First, typename... _OtherTypes>
        U32 _count_bytes(const _First& first, const _OtherTypes&... others);

        template<typename _First, typename... _OtherTypes>
        void _set(_First first, _OtherTypes... others);
    };

    // Implementation

    ArgumentList::ArgumentList() :
        _bytes(0),
        _ptr(nullptr),
        _read_it(nullptr),
        _write_it(nullptr)
    {   }

    void ArgumentList::free()
    {
        assert(_ptr);
        std::free(_ptr);
        _bytes = 0;
        _ptr = nullptr;
    }


    template<typename... _Types>
    void ArgumentList::allocate(const _Types&... types)
    {
        if (_ptr) return;
        _bytes = _count_bytes(types...);
        _ptr = std::malloc(_bytes);
        _write_it = (char*)_ptr;
        _read_it  = (char*)_ptr;
    }

    template<typename T>
    void ArgumentList::set(const T& value)
    {
        allocate(value);
        std::memcpy((void*)_write_it, &value, sizeof(T));
        _write_it += sizeof(T);
    }

    template<typename _First, typename... _Types>
    void ArgumentList::set(const _First& first, const _Types&... types)
    {
        allocate(first, types...);
        set(first);
        set(types...);
    }

    template<typename T>
    T ArgumentList::get()
    {
        assert(_read_it < _write_it);
        assert(_ptr);

        T value;
        std::memcpy(&value, (const T*)_read_it, sizeof(T));
        _read_it += sizeof(T);

        return value;
    }

    void* ArgumentList::ptr() const
    {
        return _ptr;
    }

    // Private methods
    template<typename _Last>
    U32 ArgumentList::_count_bytes(const _Last& last)
    {
        return sizeof(last);
    }

    template<typename _First, typename... _OtherTypes>
    U32 ArgumentList::_count_bytes(const _First& first, const _OtherTypes&... others)
    {
        return _count_bytes(first) + _count_bytes(others...);
    }

    template<typename _First, typename... _OtherTypes>
    void ArgumentList::_set(_First first, _OtherTypes... others)
    {
        set<_First>(first);
        set<_OtherTypes...>(others...);
    }
}