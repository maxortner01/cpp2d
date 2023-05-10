#include <cpp2d/Graphics.h>

namespace cpp2d::Buffers
{
    void AttributeBuffer::setBinding(const Binding& binding)
    {
        _binding = binding;
    }

    Binding AttributeBuffer::getBinding() const
    {
        return _binding;
    }

    void AttributeBuffer::pushAttributes(const std::initializer_list<Attribute>& list)
    {
        std::copy(list.begin(), list.end(), std::back_inserter(_attributes));
    }

    void AttributeBuffer::pushAttribute(const Attribute& attribute)
    {   
        _attributes.push_back(attribute);
    }

    const std::vector<Attribute>& AttributeBuffer::getAttributes() const
    {
        return _attributes;
    }

    Graphics::BufferHandle AttributeBuffer::getHandle() const
    {
        return _allocation->_handle;
    }
}