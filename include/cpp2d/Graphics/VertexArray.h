#pragma once

#include "../Utility.h"
#include "VertexBuffer.h"
#include "AttributeFrame.h"
#include "Surface.h"

namespace cpp2d
{
    class CPP2D_DLL VertexArray : 
        public ScopedData<Graphics::VertexBuffer>,
        public Utility::NoCopy
    {
        U32 _vertex_count;

    public:
        VertexArray(CU32& bufferCount);
        ~VertexArray();

        void setVertexCount(CU32& vertexCount);

        Graphics::AttributeFrame getAttributeFrame() const;

        void draw(const Graphics::FrameData& frameData);
    };
}