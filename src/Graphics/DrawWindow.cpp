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

    void DrawWindow::startRenderPass() 
    {
#   ifdef GDI_VULKAN
        Surface* surface = static_cast<Surface*>(this);
        VkDevice device = static_cast<VkDevice>(getFrame().command_pool.device); 
        VkFence  fence  = static_cast<VkFence>(getFrame().sync_objects.in_flight);
        vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000);
        vkResetFences(device, 1, &fence);

        U32 image_index = Graphics::GDI::get().getNextImage(_swapchain, surface->getFrame());
        setCurrentImageIndex(image_index);
#   endif

        surface->_startRenderPass();
    }

    void DrawWindow::endRenderPass()
    {
        Surface* surface = static_cast<Surface*>(this);
        surface->_endRenderPass();

#   ifdef GDI_VULKAN
        VkDevice device = static_cast<VkDevice>(getFrame().command_pool.device); 

        VkSemaphore wait_semaphore[] = {
            static_cast<VkSemaphore>(surface->getFrame().sync_objects.render_finished)
        };

        VkSwapchainKHR swapchains[] {
            static_cast<VkSwapchainKHR>(_swapchain)
        };

        U32 image_index = surface->getCurrentImageIndex();
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
        const Graphics::Frame& frame = getFrame();

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