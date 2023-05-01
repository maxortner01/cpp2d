#pragma once

#include "../Util.h"
#include <cassert>

namespace cpp2d::Utility
{
    class ArgumentList
    {
        void* _ptr;

        char* _read_it;
        char* _write_it;

        template<typename _Last>
        U32 _count_bytes(_Last last);

        template<typename _First, typename... _OtherTypes>
        U32 _count_bytes(_First first, _OtherTypes... other);

        template<typename _First, typename... _OtherTypes>
        void _set(_First first, _OtherTypes... others);

    public:
        ArgumentList();
        ~ArgumentList();

        template<typename... _Types>
        void allocate();

        template<typename T>
        void set(const T& value);

        template<typename... _Types>
        void set(_Types... types);

        template<typename T>
        T get();

        void* ptr() const;
    };

    template<typename _Last>
    U32 ArgumentList::_count_bytes(_Last last)
    {
        return sizeof(last)
    }

    template<typename _First, typename... _OtherTypes>
    U32 ArgumentList::_count_bytes(_First first, _OtherTypes... other)
    {
        return sizeof(first) + _count_bytes(other);
    }

    template<typename _First, typename... _OtherTypes>
    void ArgumentList::_set(_First first, _OtherTypes... others)
    {
        //
    }

    ArgumentList::ArgumentList() :
        _ptr(nullptr),
        _read_it(nullptr),
        _write_it(nullptr)
    {   }

    ArgumentList::~ArgumentList()
    {
        if (_ptr)
        {
            std::free(_ptr);
            _ptr = nullptr;
        }
    }

    template<typename... _Types>
    void ArgumentList::allocate()
    {
        assert(!_ptr);
        U32 size = _count_bytes<_Types>();
        _ptr = std::malloc(size);
        _write_it = (char*)_ptr;
        _read_it  = (char*)_ptr;
    }

    template<typename T>
    void ArgumentList::set(const T& value)
    {
        std::memcpy(&value, (void*)_write_it, sizeof(T));
        _write_it += sizeof(T);
    }

    template<typename... _Types>
    void ArgumentList::set(_Types... types)
    {
        allocate<_Types...>();
        
    }

    template<typename T>
    T ArgumentList::get()
    {
        T value;
        std::memcpy(&value, (void*)_read_it, sizeof(T));
        _read_it += sizeof(T);

        return value;
    }


    void* ArgumentList::ptr() const
    {
        return _ptr;
    }
}