#include <cpp2d/Graphics.h>
#include <cpp2d/Memory.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

namespace cpp2d::Graphics
{
    Frame::Frame(Graphics::GDI& gdi, Surface* parent)
    {
        command_pool = gdi.createCommandPool(this);
        command_buffers.handle = gdi.createCommandBuffer(command_pool.handle);
        command_buffers.active = false;

        sync_objects.image_avaliable = gdi.createSemaphore(this);
        sync_objects.render_finished = gdi.createSemaphore(this);
        sync_objects.in_flight = gdi.createFence(this);
    }

    Frame::~Frame()
    {
        waitUntilReady();
    }

    void Frame::waitUntilReady() const
    {
        VkDevice device = static_cast<VkDevice>(command_pool.device); 
        VkFence  fence  = static_cast<VkFence>(sync_objects.in_flight);
        vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000);
        vkResetFences(device, 1, &fence);
    }

    Surface::Surface(Graphics::GDI& gdi, const Vec2u& extent) :
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

    void Surface::create(Graphics::GDI& gdi, const Vec2u& extent, const SwapChainInfo& swapchain)
    {
        setExtent(extent);

        assert(!_frames);

        _framebuffers = (FrameImage*)std::malloc(sizeof(FrameImage) * swapchain.image_count);

        _format = swapchain.format;
        _render_pass = gdi.createRenderPass(this);

        _frames = (Frame*)std::malloc(sizeof(Frame) * _OVERLAP);
        _image_count = swapchain.image_count;
        for (U32 i = 0; i < swapchain.image_count; i++)
        {
            if (i < _OVERLAP)
                new(reinterpret_cast<void*>(&_frames[i])) Frame(gdi, this);

            _framebuffers[i].image = swapchain.images[i];
            _framebuffers[i].framebuffer = gdi.createFramebuffer(swapchain.images[i].image_view, this, this);
        }
    }

    void Surface::create(Graphics::GDI& gdi, const Vec2u& extent)
    {
        setExtent(extent);

        assert(false);
        //assert(!_frames);
    }

    Memory::ManagedObject<FrameData> Surface::startRenderPass(Graphics::GDI& gdi, Memory::Manager* manager)
    {
#   ifdef GDI_VULKAN
        //Memory::FrameManager<Memory::HeapAllocator>::get().resetWrite();

        Frame&   frame  = getFrame();
        VkDevice device = static_cast<VkDevice>(frame.command_pool.device); 

        VkCommandPool command_pool = static_cast<VkCommandPool>(frame.command_pool.handle);
        vkResetCommandPool(device, command_pool, 0);
        
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        assert(!frame.command_buffers.active);
        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frame.command_buffers.handle);
        VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error beginning command buffer (%i).", result);
            return Memory::ManagedObject<FrameData>(manager);
        }
        frame.command_buffers.active = true;

        VkClearValue clearColor = {{{0.f, 0.f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo render_pass_info {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = static_cast<VkRenderPass>(getRenderPass()),
            .framebuffer = static_cast<VkFramebuffer>(_framebuffers[_current_image_index].framebuffer),
            .renderArea = {
                .offset = {0, 0},
                .extent = { getExtent().x, getExtent().y }
            },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport {
            .x = 0,
            .y = 0,
            .width = static_cast<R32>(getExtent().x),
            .height = static_cast<R32>(getExtent().y),
            .minDepth = 0,
            .maxDepth = 1
        };

        VkRect2D scissor {
            .offset = { 0, 0 },
            .extent = { getExtent().x, getExtent().y }
        };

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
#   endif

        Memory::ManagedObject<FrameData> data(manager);
        //Memory::FrameManager<Memory::HeapAllocator>::get().request((void**)&data, sizeof(FrameData));
        data.object().command_buffer = frame.command_buffers.handle; 
        data.object().gdi            = &gdi;

        return data;
    }

    void Surface::endRenderPass(const Memory::ManagedObject<FrameData>& frameData)
    {
#   ifdef GDI_VULKAN
        const U32 index = 0;
        Frame& frame = getFrame();
        VkCommandBuffer command_buffer = static_cast<VkCommandBuffer>(frameData.object().command_buffer);
        Graphics::GDI*  gdi = frameData.object().gdi;

        vkCmdEndRenderPass(command_buffer);
        VkResult result = vkEndCommandBuffer(command_buffer);
        if (result != VK_SUCCESS)
        {
            cpp2dERROR("Error stopping command buffer recording (%i).", result);
            return;
        }
        frame.command_buffers.active = false;

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
            .pWaitDstStageMask = stage_flags,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signal_semaphores,
        };

        VkQueue queue;
        const VkFence  fence  = static_cast<VkFence>(frame.sync_objects.in_flight);
        const VkDevice device = static_cast<VkDevice>(frame.command_pool.device);
        const GDILogicDevice logic_device   = gdi->getLogicDevice(frame.command_pool.device);
        const GDIPhysicalDevice phys_device = gdi->getPhysicalDevice(logic_device.physical_device_index);
        //const QueueIndices   queue_indices  = GDI::get().getQueueIndices(phys_device);
        const U32 graphics_queue = gdi->getCurrentLogicDevice().graphics_queue;
        vkGetDeviceQueue(device, graphics_queue, 0, &queue);

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