#pragma once

#include "../Utility.h"

namespace cpp2d
{
    enum class ShaderType
    {
        Vertex,
        Fragment
    };

    enum class ShaderState
    {
        Success,
        WaitingForContent,
        CompilationError,
        CreationError
    };

    class CPP2D_DLL Shader :
        public Utility::State<ShaderState>,
        public Utility::NoCopy
    {
        Graphics::ShaderHandle _handle;
        ShaderType _type;

    public:
        Shader(const ShaderType& type);

        void fromFile(const char* filename);
        void fromString(const char* string);
        void fromBytes(const U32* const contents, U32 count);

        ShaderType getType() const;
        Graphics::ShaderHandle getHandle() const;
    };
}