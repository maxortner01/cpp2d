#pragma once

#include "../Utility.h"
#include "../util.h"

#include <string>

namespace cpp2d
{
    enum class ShaderType
    {
        Vertex,
        Fragment
    };

    enum class ShaderState
    {
        Uncompiled,
        InProgress,
        CompileError,
        CompileComplete,
        Linking,
        LinkingError,
        LinkingComplete
    };

    class Shader :
        public Utility::State<ShaderState>
    {
        const uint32_t _shader_count;
        uint32_t    _id;
        uint32_t*   _ids;
        ShaderType* _types;

    public:
        Shader(const ShaderType* const types, const uint32_t& shaderCount);
        ~Shader();

        int32_t typeIndex(const ShaderType& type) const;

        void fromFile  (const ShaderType& type, const std::string& filename);
        void fromString(const ShaderType& type, const std::string& contents);

        void link();

        void   bind() const;
        void unbind() const;
    };
}