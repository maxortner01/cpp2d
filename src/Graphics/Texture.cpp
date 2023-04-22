#include <cpp2d/Graphics.h>

#include <GL/glew.h>

namespace cpp2d
{
    Texture::Texture() :
        UnsignedSizable({ 0, 0 }), _id(0)
    {   }

    Texture::Texture(const std::string& filename) :
        UnsignedSizable({ 0, 0 }), _id(0)
    {
        fromFile(filename);
    }

    Texture::Texture(const Vec2u& size) :
        UnsignedSizable(size), _id(0)
    {
        create(size);
    }

    Texture::~Texture()
    {
        if (_id)
        {
            glDeleteTextures(1, &_id);
            _id = 0;
        }
    }

    void Texture::create(const Vec2u& size)
    {
        assert(!_id);
        setSize(size);

        glGenTextures(1, &_id);
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        unbind();
    }

    void Texture::fromFile(const std::string& filename) 
    {
        stbi_set_flip_vertically_on_load(true);

        int x, y, comp;
        void* data = stbi_load(filename.c_str(), &x, &y, &comp, STBI_rgb_alpha);
        assert(data);

        create({ (uint32_t)x, (uint32_t)y });

        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getSize().x, getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        unbind();

        stbi_image_free(data);
    }

    uint32_t Texture::getID() const
    {
        return _id;
    }

    void Texture::bind(const uint32_t& layer)
    {
        _current_layer = layer;
        glActiveTexture(GL_TEXTURE0 + layer);
        glBindTexture(GL_TEXTURE_2D, _id);
    }

    void Texture::unbind()
    {
        glActiveTexture(GL_TEXTURE0 + _current_layer);
        glBindTexture(GL_TEXTURE_2D, 0);
        _current_layer = 0;
    }
}