#include <cpp2d/Graphics.h>

namespace cpp2d::Graphics
{
    Frame::Frame(GDIImage _image, Surface* parent) :
        image(_image)
    {
        framebuffer = GDI::get().createFramebuffer(image.image_view, parent, this);
        command_pool = GDI::get().createCommandPool(this);
    }

    Surface::Surface(const Vec2u& extent) :
        Sizable(extent),
        _frames(nullptr),
        _frame_count(0)
    {   }

    Surface::~Surface()
    {   
        if (_frames)
        {
            for (U32 i = 0; i < _frame_count; i++)
                _frames[i].~Frame();

            std::free(_frames);
            _frames = nullptr;
        }
    }

    void Surface::create(const Vec2u& extent, const SwapChainInfo& swapchain)
    {
        setExtent(extent);

        assert(!_frames);

        _format = swapchain.format;
        _render_pass = Graphics::GDI::get().createRenderPass(this);

        _frame_count = swapchain.image_count;
        _frames = (Frame*)std::malloc(sizeof(Frame) * swapchain.image_count);
        for (U32 i = 0; i < swapchain.image_count; i++)
            new(reinterpret_cast<void*>(&_frames[i])) Frame(swapchain.images[i], this);
    }

    void Surface::create(const Vec2u& extent)
    {
        setExtent(extent);

        assert(false);
        //assert(!_frames);
    }

    void Surface::startRenderPass()
    {

    }

    void Surface::endRenderPass()
    {
        
    }
    
    Frame& Surface::getFrame(const U32& index) const
    {
        assert(index < _frame_count);
        return _frames[index];
    }

    FormatHandle Surface::getFormat() const
    {
        return _format;
    }

    RenderPassHandle Surface::getRenderPass() const
    {
        return _render_pass;
    }
}