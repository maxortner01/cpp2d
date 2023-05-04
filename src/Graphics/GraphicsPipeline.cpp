#include <cpp2d/Graphics.h>

#define SHADER_TYPE_NOT_IN_PIPELINE false

namespace cpp2d
{
    GraphicsPipeline::GraphicsPipeline(std::initializer_list<ShaderType> shaders, const Graphics::AttributeFrame& attributes) :
        Utility::State<GraphicsPipelineState>(GraphicsPipelineState::NotBuilt),
        _attributes(attributes),
        _shaders(shaders.size())
    {
        for (U32 i = 0; i < _shaders.size(); i++)
            _shaders[i] = new Shader(*(shaders.begin() + i));
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        for (U32 i = 0; i < _shaders.size(); i++)
            delete _shaders[i];        
    }

    Shader& GraphicsPipeline::getShader(const ShaderType& type)
    {
        for (U32 i = 0; i < _shaders.size(); i++)
            if (_shaders[i]->getType() == type)
                return *_shaders[i];

        assert(SHADER_TYPE_NOT_IN_PIPELINE);
    }

    void GraphicsPipeline::create()
    {
        assert(isComplete());
        assert(getState() == GraphicsPipelineState::NotBuilt);
    }

    bool GraphicsPipeline::isComplete() const
    {
        for (U32 i = 0; i < _shaders.size(); i++)
            if (_shaders[i]->getState() != ShaderState::Success)
                return false;

        return true;
    }
}

#undef SHADER_TYPE_NOT_IN_PIPELINE