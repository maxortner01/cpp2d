#include <cpp2d/Graphics.h>

#include <string>
#include <fstream>

#if defined(USE_SHADERC) && defined(GDI_VULKAN)
#   include <shaderc/shaderc.hpp>
#   define USE_SHADERC_AND_VULKAN
#endif

namespace cpp2d
{
    Shader::Shader(const ShaderType& type) :
        Utility::State<ShaderState>(ShaderState::WaitingForContent),
        _type(type)
    {   }

    void Shader::fromFile(const char* filename)
    {
        std::ifstream file(filename);
        assert(file);

        std::string content(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>())
        );

        fromString(content.c_str());
    }

    void Shader::fromString(const char* string)
    {
#   ifdef USE_SHADERC_AND_VULKAN
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        shaderc_shader_kind kind;
        switch (_type)
        {
        case ShaderType::Vertex:    kind = shaderc_glsl_vertex_shader;   break;
        case ShaderType::Fragment:  kind = shaderc_glsl_fragment_shader; break;
        };

        options.SetOptimizationLevel(shaderc_optimization_level_size);
        auto pp = compiler.PreprocessGlsl(string, strlen(string), kind, "cpp2d", options);
        auto compiling = compiler.CompileGlslToSpv(string, kind, "cpp2d", options);

        if (compiling.GetNumErrors() > 0)
        {
            setState(ShaderState::CompilationError);
            cpp2dERROR("Error compiling shader:\n%s", compiling.GetErrorMessage().c_str());
            return;
        }

        cpp2dINFO("Successfully compiled shader from GLSL code.");

        fromBytes(compiling.begin(), std::distance(compiling.begin(), compiling.end()));
#   endif

#   if !defined(USE_SHADERC) && defined(GDI_VULKAN)
        cpp2dFATAL("Shaderc not loaded, can't load shaders from string!");
#   endif
    }

    void Shader::fromBytes(const U32* contents, U32 count)
    {
        _handle = Graphics::GDI::get().createShader(contents, count);
        
        if (!_handle)
            setState(ShaderState::CreationError);
        else
            setState(ShaderState::Success);
    }

    ShaderType Shader::getType() const
    {
        return _type;
    }

    Graphics::ShaderHandle Shader::getHandle() const
    {
        return _handle;
    }
}