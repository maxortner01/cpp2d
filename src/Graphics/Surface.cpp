#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

namespace cpp2d::Graphics
{
    Frame::Frame(GDIImage _image, Surface* parent) :
        image(_image)
    {
        framebuffer = GDI::get().createFramebuffer(image.image_view, parent, this);
        command_pool = GDI::get().createCommandPool(this);
        command_buffers = GDI::get().createCommandBuffer(command_pool.handle);
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

    void Surface::startRenderPass(const U32& frameIndex)
    {
        assert(frameIndex < _frame_count);
        _current_frame = frameIndex;

#   ifdef GDI_VULKAN
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        const Frame& frame = getFrame(frameIndex);    
        VkCommandPool   command_pool = static_cast<VkCommandPool>(frame.command_pool.handle);
        VkDevice        device       = static_cast<VkDevice>(frame.command_pool.device); 
        vkResetCommandPool(device, command_pool, 0);

        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frame.command_buffers);

        VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error beginning command buffer (%i).", result);
            return;
        }

        VkClearValue clearColor = {{{1.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo render_pass_info {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = static_cast<VkRenderPass>(getRenderPass()),
            .framebuffer = static_cast<VkFramebuffer>(frame.framebuffer),
            .renderArea.offset = {0, 0},
            .renderArea.extent = { getExtent().x, getExtent().y },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
#   endif
    }

    void Surface::endRenderPass()
    {
        const U32 index = 0;
        const Frame& frame = getFrame(_current_frame);
        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frame.command_buffers);

        vkCmdEndRenderPass(command_buffer);
        VkResult result = vkEndCommandBuffer(command_buffer);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error stopping command buffer recording (%i).", result);
            return;
        }
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