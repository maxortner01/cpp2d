#include <CPP2D/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    DrawTexture::DrawTexture(const Vec2u& size) :
        UnsignedSizable(size), _id(0)
    {
        create(size);
    }

    DrawTexture::~DrawTexture()
    {
        if (_id)
        {
            glDeleteFramebuffers(1, &_id);
            _id = 0;
        }
    }

    void DrawTexture::clear()
    {
        bind();
        glClearColor(84.f / 255.f, 51.f / 255.f, 68.f / 255.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        unbind();
    }

    Texture& DrawTexture::getTexture()
    {
        return _color;
    }

    Vec2u DrawTexture::getSize() const
    {
        return ((UnsignedSizable*)this)->getSize();
    }

    void DrawTexture::create(const Vec2u& size)
    {
        assert(!_id);

        setSize(size);
        glGenFramebuffers(1, &_id);

        bind();

        _color.create(size);
        _color.bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _color.getID(), 0);
        _color.unbind();

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

        unbind();
    }

    void DrawTexture::bind() 
    {
        int32_t viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        if (viewport[2] != _last_dimensions.x || viewport[3] != _last_dimensions.y)
            _last_dimensions = Vec2u(viewport[2], viewport[3]);

        glBindFramebuffer(GL_FRAMEBUFFER, _id);
        glViewport(0, 0, getSize().x, getSize().y);
    }

    void DrawTexture::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (_last_dimensions.x != 0 && _last_dimensions.y != 0)
            glViewport(0, 0, _last_dimensions.x, _last_dimensions.y);
    }
}