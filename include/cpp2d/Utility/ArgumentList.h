#pragma once

#include "../Util.h"
#include <cassert>
#include <iostream>

namespace cpp2d::Utility
{
    class ArgumentList
    {
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

    public:
        ArgumentList();

        void free();

        template<typename... _Types>
        void allocate(const _Types&... types);

        template<typename T>
        void set(const T& value);

        template<typename _First, typename... _Types>
        void set(const _First& first, const _Types&... types);

        template<typename T>
        T get();

        void* ptr() const;
    };

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
        set<_OtherTypes>(others);
    }

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
}