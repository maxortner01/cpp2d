#pragma once

#include "AttributeFrame.h"
#include "../Utility.h"
#include <initializer_list>

namespace cpp2d
{
    class Shader;
    enum class ShaderType;

    enum class GraphicsPipelineState
    {
        Success,
        NotBuilt
    };

    class GraphicsPipeline :
        public Utility::State<GraphicsPipelineState>,
        public Utility::NoCopy
    {
        ScopedData<Shader*> _shaders;
        Graphics::AttributeFrame _attributes;

    public:
        GraphicsPipeline(std::initializer_list<ShaderType> shaders, const Graphics::AttributeFrame& attributes);
        ~GraphicsPipeline();

        Shader& getShader(const ShaderType& type);
        void create();    

        bool isComplete() const;
    };
}