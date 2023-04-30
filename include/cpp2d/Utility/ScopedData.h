#pragma once

#include "../Util.h"

namespace cpp2d
{
    template<typename T>
    class ScopedData
    {
        CU32 _size;
        T*   _data;

    public:
        ScopedData(CU32& count);
        ~ScopedData();

        T*  ptr()  const;
        U32 size() const;

        T& operator[](CU32& index) const;
    };

    // Implementation
    template<typename T>
    ScopedData<T>::ScopedData(CU32& count) :
        _size(count)
    {
        _data = (T*)std::malloc(sizeof(T) * count);
    }

    template<typename T>
    ScopedData<T>::~ScopedData()
    {
        assert(_data);
        std::free(_data);
        _data = nullptr;
    }

    template<typename T>
    T* ScopedData<T>::ptr() const
    { return _data; }

    template<typename T>
    U32 ScopedData<T>::size() const
    { return _size; }

    template<typename T>
    T& ScopedData<T>::operator[](CU32& index) const
    {
        assert(index < _size);
        return _data[index];
    }
}