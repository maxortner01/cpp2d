#pragma once

#include "../Util.h"

namespace cpp2d
{
    /**
     * @class ScopedData
     * @brief A class that provides scoped access to a block of data
     *
     * This template class provides a way to encapsulate a block of data and provide
     * scoped access to it. The data is dynamically allocated and automatically
     * deallocated when the object goes out of scope.
     *
     * @tparam T The type of data to encapsulate
     */
    template<typename T>
    class ScopedData
    {
    public:
        /**
         * @brief Constructs a ScopedData object with the given size
         * @param count The number of elements to allocate for the data block
         *
         * This constructor allocates a block of memory to store the given number of
         * elements of type T. The memory is dynamically allocated and
         * automatically deallocated when the ScopedData object goes out of scope.
         */
        ScopedData(CU32& count);

        /**
         * @brief Constructs a ScopedData object as a copy of another ScopedData object
         * @param data The ScopedData object to copy
         *
         * This constructor creates a new ScopedData object that is a copy of the given
         * ScopedData object. The new object has its own dynamically allocated memory
         * block to store the data, which is a copy of the data in the original object.
         */
        ScopedData(const ScopedData<T>& data);

        /**
         * @brief Destroys the ScopedData object and frees the associated memory
         *
         * This destructor frees the dynamically allocated memory block that was used
         * to store the data.
         */
        ~ScopedData();

        /**
         * @brief Assigns the data from another ScopedData object to this object
         * @param data The ScopedData object to copy
         * @return A reference to this ScopedData object
         *
         * This assignment operator copies the data from the given ScopedData object
         * into this object. The existing data in this object is freed and replaced
         * with a new dynamically allocated memory block that contains a copy of the
         * data in the other object.
         */
        ScopedData<T>& operator=(const ScopedData<T>& data);

        /**
         * @brief Returns a pointer to the data block
         * @return A pointer to the beginning of the data block
         *
         * This method returns a pointer to the beginning of the dynamically allocated
         * data block that was created by the constructor. The pointer can be used to
         * access the individual elements of the data block.
         */
        T* ptr() const;

        /**
         * @brief Returns the size of the data block
         * @return The number of elements in the data block
         *
         * This method returns the number of elements that were allocated in the
         * dynamically allocated data block.
         */
        U32 size() const;

        /**
         * @brief Returns a reference to an element of the data block
         * @param index The index of the element to access
         * @return A reference to the element at the given index
         *
         * This method returns a reference to the element of the data block at the
         * given index. The index must be less than the size of the data block.
         */
        T& operator[](CU32& index) const;

        T& get(CU32& index) const;

    private:
        CU32 _size; 
        T* _data;   
    };


    // Implementation
    template<typename T>
    ScopedData<T>::ScopedData(CU32& count) :
        _size(count),
        _data(nullptr)
    {
        if (count)
            _data = (T*)std::malloc(sizeof(T) * count);
    }

    template<typename T>
    ScopedData<T>::ScopedData(const ScopedData<T>& data) :
        ScopedData(data._size)
    {
        std::memcpy(_data, data._data, sizeof(T) * data._size);
    }

    template<typename T>
    ScopedData<T>::~ScopedData()
    {
        assert(_data);
        std::free(_data);
        _data = nullptr;
    }

    template<typename T>
    ScopedData<T>& ScopedData<T>::operator=(const ScopedData<T>& data)
    {
        assert(_data);
        std::free(_data);
        _data = (T*)std::malloc(sizeof(T) * data._size);
        std::memcpy(_data, data._data, sizeof(T) * data._size);
        _size = data._size;
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

    template<typename T>
    T& ScopedData<T>::get(CU32& index) const
    {
        assert(index < _size);
        return _data[index];
    }
}