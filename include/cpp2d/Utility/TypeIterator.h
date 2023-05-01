#pragma once

namespace cpp2d::Utility
{
    class TypeIterator
    {
        char* it;

    public:
        TypeIterator(void* ptr);

        template<typename T>
        T get();
    };

    TypeIterator::TypeIterator(void* ptr) :
        it((char*)ptr)
    {   }

    template<typename T>
    T TypeIterator::get()
    {
        T t = *(T*)it;
        it += sizeof(T);
        return t;
    }
}