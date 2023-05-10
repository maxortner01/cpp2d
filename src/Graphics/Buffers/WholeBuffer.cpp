#include <cpp2d/Graphics.h>

namespace cpp2d::Buffers
{
    WholeBuffer::WholeBuffer()
    {
        _allocation = new Allocation();
    }

    WholeBuffer::~WholeBuffer()
    {
        if (_allocation)
        {
            delete _allocation;
            _allocation = nullptr;
        }
    }

    void WholeBuffer::setData(const void* data, CU32& bytes)
    {
        assert(_allocation);
        //_allocation->setData(data, bytes, 0);
    }
}