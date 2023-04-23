#pragma once

#include "../Utility.h"

namespace cpp2d
{
    class Texture :
        public UnsignedSizable
    {
        uint32_t _id;
        uint32_t _current_layer;

    public:
        Texture();
        Texture(const std::string& filename);
        Texture(const Vec2u& size);
        ~Texture();

        void create(const Vec2u& size);

        void fromFile(const std::string& filename);

        static void bindTexture(const uint32_t& id, const uint32_t& layer);

        uint32_t getID() const;
        void bind(const uint32_t& layer = 0);
        void unbind();
    };
}