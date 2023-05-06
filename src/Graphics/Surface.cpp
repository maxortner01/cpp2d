#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

namespace cpp2d::Graphics
{
    Frame::Frame(Surface* parent)
    {
        command_pool = GDI::get().createCommandPool(this);
        command_buffers = GDI::get().createCommandBuffer(command_pool.handle);

        sync_objects.image_avaliable = GDI::get().createSemaphore(this);
        sync_objects.render_finished = GDI::get().createSemaphore(this);
        sync_objects.in_flight = GDI::get().createFence(this);
    }

    Surface::Surface(const Vec2u& extent) :
        Sizable(extent),
        _frames(nullptr),
        _image_count(0),
        _current_frame(0),
        _framebuffers(nullptr),
        _current_image_index(0)
    {   }

    Surface::~Surface()
    {   
        if (_frames)
        {
            for (U32 i = 0; i < _OVERLAP; i++)
                _frames[i].~Frame();

            std::free(_frames);
            _frames = nullptr;
        }

        if (_framebuffers)
        {
            std::free(_framebuffers);
            _framebuffers = nullptr;
        }
    }

    void Surface::create(const Vec2u& extent, const SwapChainInfo& swapchain)
    {
        setExtent(extent);

        assert(!_frames);

        _framebuffers = (_Framebuffer*)std::malloc(sizeof(_Framebuffer) * swapchain.image_count);

        _format = swapchain.format;
        _render_pass = Graphics::GDI::get().createRenderPass(this);

        _frames = (Frame*)std::malloc(sizeof(Frame) * _OVERLAP);
        _image_count = swapchain.image_count;
        for (U32 i = 0; i < swapchain.image_count; i++)
        {
            if (i < _OVERLAP)
                new(reinterpret_cast<void*>(&_frames[i])) Frame(this);

            _framebuffers[i].image = swapchain.images[i];
            _framebuffers[i].framebuffer = GDI::get().createFramebuffer(swapchain.images[i].image_view, this, this);
        }
    }

    void Surface::create(const Vec2u& extent)
    {
        setExtent(extent);

        assert(false);
        //assert(!_frames);
    }

    void Surface::_startRenderPass()
    {
#   ifdef GDI_VULKAN
        const Frame& frame  = getFrame();
        VkDevice     device = static_cast<VkDevice>(frame.command_pool.device); 
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        VkCommandPool command_pool = static_cast<VkCommandPool>(frame.command_pool.handle);
        vkResetCommandPool(device, command_pool, 0);

        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frame.command_buffers);

        VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error beginning command buffer (%i).", result);
            return;
        }

        float x = 0.5 * (sin(_current_frame * 0.01) + 1.f);
        float y = 0.5 * (cos(_current_frame * 0.01) + 1.f);
        VkClearValue clearColor = {{{x, y, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo render_pass_info {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = static_cast<VkRenderPass>(getRenderPass()),
            .framebuffer = static_cast<VkFramebuffer>(_framebuffers[_current_image_index].framebuffer),
            .renderArea.offset = {0, 0},
            .renderArea.extent = { getExtent().x, getExtent().y },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
#   endif
    }

    void Surface::_endRenderPass()
    {
#   ifdef GDI_VULKAN
        const U32 index = 0;
        const Frame& frame = getFrame();
        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frame.command_buffers);

        vkCmdEndRenderPass(command_buffer);
        VkResult result = vkEndCommandBuffer(command_buffer);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error stopping command buffer recording (%i).", result);
            return;
        }

        VkSemaphore wait_semaphores[] = {
            static_cast<VkSemaphore>(frame.sync_objects.image_avaliable)
        };

        VkSemaphore signal_semaphores[] = {
            static_cast<VkSemaphore>(frame.sync_objects.render_finished)
        };

        VkPipelineStageFlags stage_flags[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSubmitInfo submit_info {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = wait_semaphores,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signal_semaphores,
            .pWaitDstStageMask = stage_flags,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffer
        };

        VkQueue queue;
        const VkFence  fence  = static_cast<VkFence>(frame.sync_objects.in_flight);
        const VkDevice device = static_cast<VkDevice>(frame.command_pool.device);
        GDILogicDevice logicDevice = Graphics::GDI::get().getLogicDevice();
        vkGetDeviceQueue(device, logicDevice.graphics_queue, 0, &queue);

        result = vkQueueSubmit(
            queue,
            1,
            &submit_info,
            fence
        );

        if (result != VK_SUCCESS)
            cpp2dERROR("Queue submit failed (%i)", result);
#   endif

        _current_frame++;
    }
    
    Frame& Surface::getFrame() const
    {
        return _frames[_current_frame % _OVERLAP];
    }

    U32 Surface::getCurrentImageIndex() const
    {
        return _current_image_index;
    }

    void Surface::setCurrentImageIndex(const U32& index)
    {
        assert(index < _image_count);
        _current_image_index = index;
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