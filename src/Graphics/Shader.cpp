#include <cpp2d/Graphics.h>

#include <iostream>
#include <fstream>
#include <GL/glew.h>

namespace cpp2d
{
    Shader::Shader(std::initializer_list<ShaderType> type) :
        Shader(type.begin(), type.size())
    {   }

    Shader::Shader(const ShaderType* const types, const uint32_t& shaderCount) :
        Utility::State<ShaderState>(ShaderState::Uncompiled),
        _shader_count(shaderCount),
        _ids(nullptr), _types(nullptr), _id(glCreateProgram())
    {
        _ids   = (uint32_t*)  std::malloc(sizeof(uint32_t)   * shaderCount);
        _types = (ShaderType*)std::malloc(sizeof(ShaderType) * shaderCount);
        std::memcpy(_types, types, sizeof(ShaderType) * shaderCount);
    }

    Shader::~Shader()
    {
        if (_ids)
            glDeleteProgram(_id);

        FREE_CHECK(_ids);
        FREE_CHECK(_types);
    }

    int32_t Shader::typeIndex(const ShaderType& type) const
    {
        for (uint32_t i = 0; i < _shader_count; i++)
            if (_types[i] == type) return i;

        return -1;
    }

    void Shader::fromFile(const ShaderType& type, const std::string& filename) 
    {
        // Make sure the requested shader type exists
        assert(typeIndex(type) != -1);

        setState(ShaderState::InProgress);

        std::ifstream file(filename);

        // Make sure file exists
        assert(file);

        // Go through the lines (probably a better way to do this)
        std::string contents;
        for (std::string line; std::getline(file, line);)
            contents += line + "\n";

        file.close();

        // Pass the contents over to the string parser to generate shader
        fromString(type, contents);
    }

    void Shader::fromString(const ShaderType& type, const std::string& contents)
    {
        // Make sure the requested shader type exists
        int32_t id_index = typeIndex(type);
        assert(id_index != -1);

        setState(ShaderState::InProgress);

        std::string type_string;
        GLenum gl_type;
        switch(type)
        {
        case ShaderType::Vertex:   gl_type = GL_VERTEX_SHADER;   type_string = "vertex";   break;
        case ShaderType::Fragment: gl_type = GL_FRAGMENT_SHADER; type_string = "fragment"; break;
        }

        _ids[id_index] = glCreateShader(gl_type);

        const char*   c_contents = contents.c_str();
        const int32_t c_length   = contents.length();

        glShaderSource(_ids[id_index], 1, &c_contents, &c_length);
        glCompileShader(_ids[id_index]);

        int success;
        glGetShaderiv(_ids[id_index], GL_COMPILE_STATUS, &success);
        if (!success)
        {
            setState(ShaderState::CompileError);

            char infoLog[512];
            glGetShaderInfoLog(_ids[id_index], 512, nullptr, infoLog);\

            std::cout << "Error compiling " << type_string << " shader.\n" << infoLog << "\n";
        }

        glAttachShader(_id, _ids[id_index]);

        setState(ShaderState::CompileComplete);
    }

    void Shader::link()
    {
        setState(ShaderState::Linking);
        glLinkProgram(_id);

        int success;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            setState(ShaderState::LinkingError);

            char infoLog[512];
            glGetProgramInfoLog(_id, 512, nullptr, infoLog);
            std::cout << "Error linking program!\n" << infoLog << "\n";
        }

        for (int i = 0; i < _shader_count; i++)
            glDeleteShader(_ids[i]);

        FREE_CHECK(_ids);
    }

    void Shader::bind() const
    {
        glUseProgram(_id);
    }

    void Shader::unbind() const
    {
        glUseProgram(0);
    }

    void Shader::setUniform(const std::string& uniformName, const float& uniform) const
    {
        glUniform1f(glGetUniformLocation(_id, uniformName.c_str()), uniform);
    }
    
    void Shader::setUniform(const std::string& uniformName, const Vec2f& uniform) const
    {
        glUniform2f(glGetUniformLocation(_id, uniformName.c_str()), uniform.x, uniform.y);
    }
    
    void Shader::setUniform(const std::string& uniformName, const int32_t& uniform) const
    {
        glUniform1i(glGetUniformLocation(_id, uniformName.c_str()), uniform);
    }
        
    void Shader::setUniform(const std::string& uniformName, const uint32_t* uniform, const size_t& count) const
    {
        glUniform1uiv(glGetUniformLocation(_id, uniformName.c_str()), count, uniform);
    }
}