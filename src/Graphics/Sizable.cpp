#include <CPP2D/Graphics.h>

namespace cpp2d
{
    template<typename T>
    Sizable<T>::Sizable(const T& size) :
        _size(size)
    {   }

    template<typename T>
    void Sizable<T>::setSize(const T& size)
    {
        _size = size;
    }

    template<typename T>
    T Sizable<T>::getSize() const
    {
        return _size;
    }
}