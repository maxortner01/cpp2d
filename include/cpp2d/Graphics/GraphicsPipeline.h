#pragma once

#include "AttributeFrame.h"
#include "../Utility.h"
#include <initializer_list>

namespace cpp2d
{
namespace Graphics
{
    class Surface;
}

    class Shader;
    enum class ShaderType;

    enum class GraphicsPipelineState
    {
        Success,
        NotBuilt,
        Failed
    };

    class GraphicsPipeline :
        public Utility::State<GraphicsPipelineState>,
        public Utility::NoCopy
    {
        Graphics::GDIPipeline _pipeline;

        ScopedData<Shader*> _shaders;
        Graphics::AttributeFrame _attributes;
        Graphics::Surface* _surface;

    public:
        GraphicsPipeline(std::initializer_list<ShaderType> shaders, Graphics::Surface& surface, const Graphics::AttributeFrame& attributes);
        ~GraphicsPipeline();

        Shader& getShader(const ShaderType& type);
        void create();    

        bool shadersComplete() const;

        void bind(const Graphics::CommandBufferHandle& commandBuffer);
    };
}