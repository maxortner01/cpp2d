#include <cpp2d/Graphics.h>

#ifdef GDI_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <GLFW/glfw3.h>

namespace cpp2d
{
    DrawWindow::DrawWindow(Graphics::GDI& gdi, CU32& width, CU32& height, const char* title) :
        Window(width, height, title),
        Surface(gdi, {width, height})
    {
        Window* window = (Window*)this;

        I32 _width, _height;
        glfwGetFramebufferSize((GLFWwindow*)window->getHandle(), &_width, &_height);
        setExtent({ (U32)_width, (U32)_height });

        _handle = gdi.getSurfaceHandle(this);

        Graphics::SwapChainInfo info = gdi.createSwapChain(this);
        _swapchain = info.handle;

        create(gdi, { (U32)_width, (U32)_height }, info);
        std::free(info.images);
    }

    DrawWindow::~DrawWindow()
    {   }

    void DrawWindow::display() const
    {
        // swap buffers vulkan version
    }

    Memory::ManagedObject<Graphics::FrameData> DrawWindow::beginFrame(Graphics::GDI& gdi, Memory::Manager* manager) 
    {
        const Graphics::Frame& frame = getFrame();
        frame.waitUntilReady();

        U32 image_index = gdi.getNextImage(_swapchain, getFrame());
        setCurrentImageIndex(image_index);

        return startRenderPass(gdi, manager);
    }

    void DrawWindow::endFrame(const Memory::ManagedObject<Graphics::FrameData>& frameData)
    {
        endRenderPass(frameData);

        const Graphics::Frame& frame = getFrame();
        Graphics::GDI*         gdi   = frameData.object().gdi;

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

        U32 present_index = gdi->getCurrentLogicDevice().present_queue;

        VkQueue queue;
        Graphics::GDILogicDevice logicDevice = gdi->getLogicDevice();
        vkGetDeviceQueue(device, present_index, 0, &queue);

        vkQueuePresentKHR(queue, &present_info);
#   endif
    }

    Graphics::SurfaceHandle DrawWindow::getSurfaceHandle() const
    {
        return _handle;
    }
}