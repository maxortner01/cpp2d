#pragma once

#include "./Buffers/AttributeBuffer.h"
#include "../Utility.h"
#include <initializer_list>

namespace cpp2d
{
namespace Graphics
{
    struct FrameData;
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

    class CPP2D_DLL GraphicsPipeline :
        public Utility::State<GraphicsPipelineState>,
        public Utility::NoCopy
    {
        U32 _push_constants_size;

        Graphics::GDIPipeline _pipeline;

        ScopedData<Shader*> _shaders;
        Buffers::AttributeFrame _attributes;
        Graphics::Surface* _surface;

        void _set_push_constants(const Graphics::FrameData* frame_data, const void* data);

    public:
        GraphicsPipeline(std::initializer_list<ShaderType> shaders, Graphics::Surface& surface, const Buffers::AttributeFrame& attributes);
        ~GraphicsPipeline();

        Shader& getShader(const ShaderType& type);
        void create();    

        bool shadersComplete() const;

        void bind(const Graphics::FrameData* frameData);

        template<typename T>
        void setPushConstants()
        {
            _push_constants_size = sizeof(T);
        }

        template<typename T>
        void setPushConstantData(const Graphics::FrameData* frameData, const T& data)
        {
            assert(sizeof(T) == _push_constants_size);
            _set_push_constants(frameData, &data);
        }
    };
}