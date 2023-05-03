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
        CompilationError
    };

    class CPP2D_DLL Shader :
        public Utility::State<ShaderState>
    {
    public:
        Shader();

        void fromFile(const ShaderType& type, const char* filename);
        void fromString(const ShaderType& type, const char* string);
        void fromBytes(const ShaderType& type, const U32* const contents, U32 count);
    };
}