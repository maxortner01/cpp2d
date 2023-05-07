#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <GLFW/glfw3.h>

namespace cpp2d
{
    DrawWindow::DrawWindow(CU32& width, CU32& height, const char* title) :
        Window(width, height, title),
        Surface({width, height})
    {
        Window* window = (Window*)this;

        I32 _width, _height;
        glfwGetFramebufferSize((GLFWwindow*)window->getHandle(), &_width, &_height);
        setExtent({ (U32)_width, (U32)_height });

        _handle = Graphics::GDI::get().getSurfaceHandle(this);

        Graphics::SwapChainInfo info = Graphics::GDI::get().createSwapChain(this);
        _swapchain = info.handle;

        create({ (U32)_width, (U32)_height }, info);
        std::free(info.images);
    }

    DrawWindow::~DrawWindow()
    {   }

    void DrawWindow::display() const
    {
        // swap buffers vulkan version
    }

    Graphics::CommandBufferHandle DrawWindow::beginFrame() 
    {
        const Graphics::Frame& frame = getFrame();
        frame.waitUntilReady();

        U32 image_index = Graphics::GDI::get().getNextImage(_swapchain, getFrame());
        setCurrentImageIndex(image_index);

        return startRenderPass();
    }

    void DrawWindow::endFrame(const Graphics::CommandBufferHandle& commandBuffer)
    {
        endRenderPass(commandBuffer);

        const Graphics::Frame& frame = getFrame();
#   ifdef GDI_VULKAN
        VkDevice device = static_cast<VkDevice>(frame.command_pool.device); 

        VkSemaphore wait_semaphore[] = {
            static_cast<VkSemaphore>(frame.sync_objects.render_finished)
        };

        VkSwapchainKHR swapchains[] {
            static_cast<VkSwapchainKHR>(_swapchain)
        };

        U32 image_index = getCurrentImageIndex();
        VkPresentInfoKHR present_info {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = wait_semaphore,
            .swapchainCount = 1,
            .pSwapchains = swapchains,
            .pImageIndices = &image_index,
            .pResults = nullptr
        };

        U32 present_index = Graphics::GDI::get().getPresentIndex(this);

        VkQueue queue;
        Graphics::GDILogicDevice logicDevice = Graphics::GDI::get().getLogicDevice();
        vkGetDeviceQueue(device, present_index, 0, &queue);

        vkQueuePresentKHR(queue, &present_info);
#   endif
    }

    Graphics::SurfaceHandle DrawWindow::getSurfaceHandle() const
    {
        return _handle;
    }
}