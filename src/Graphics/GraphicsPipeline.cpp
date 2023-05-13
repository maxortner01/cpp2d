#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

#define SHADER_TYPE_NOT_IN_PIPELINE false

namespace cpp2d
{

    void GraphicsPipeline::_set_push_constants(const Graphics::FrameData* frame_data, const void* data)
    {
        auto command_buffer = static_cast<VkCommandBuffer>(frame_data->command_buffer);
        auto layout         = static_cast<VkPipelineLayout>(_pipeline.layout);

        vkCmdPushConstants(command_buffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, _push_constants_size, data);
    }

    GraphicsPipeline::GraphicsPipeline(std::initializer_list<ShaderType> shaders, Graphics::Surface& surface, const Buffers::AttributeFrame& attributes) :
        Utility::State<GraphicsPipelineState>(GraphicsPipelineState::NotBuilt),
        _attributes(attributes),
        _surface(&surface),
        _shaders(shaders.size()),
        _push_constants_size(0)
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
        assert(shadersComplete());
        assert(getState() == GraphicsPipelineState::NotBuilt);

        _pipeline = Graphics::GDI::get().createPipeline(
            _shaders, _surface, _attributes, _push_constants_size
        );

        if (_pipeline.layout)
            setState(GraphicsPipelineState::Success);
        else
            setState(GraphicsPipelineState::Failed);
    }

    bool GraphicsPipeline::shadersComplete() const
    {
        for (U32 i = 0; i < _shaders.size(); i++)
            if (_shaders[i]->getState() != ShaderState::Success)
                return false;

        return true;
    }

    void GraphicsPipeline::bind(const Graphics::FrameData* frameData)
    {
#   ifdef GDI_VULKAN
        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frameData->command_buffer);

        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            static_cast<VkPipeline>(_pipeline.handle)
        );
#   endif
    }
}

#undef SHADER_TYPE_NOT_IN_PIPELINE