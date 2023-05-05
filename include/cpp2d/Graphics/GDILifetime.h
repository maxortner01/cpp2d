#pragma once

#include "../Utility.h"
#include <stack>

namespace cpp2d::Graphics
{
    enum class GDIObject
    {
        Instance,
        DebugMessenger,
        Device,
        Surface,
        Swapchain,
        ImageView,
        Shader,
        PipelineLayout,
        Renderpass,
        GraphicsPipeline,
        Framebuffer,
        CommandPool
    };

    struct GDIObjectInstance
    {
        GDIObject type;
        void*     handle;
        Utility::ArgumentList arguments;
    };

    /**
     * @class GDILifetime
     *
     * @brief Represents the lifetime of GDI objects.
     *
     * This class manages the lifetime of GDI objects by keeping a stack of GDIObjectInstance instances. 
     * When an instance of this class is destroyed, all GDI objects in the stack are automatically released.
     */
    class GDILifetime
    {
    protected:
        /**
         * @brief The stack of GDIObjectInstance instances.
         */
        std::stack<GDIObjectInstance> _objects;

    public:
        /**
         * @brief Destroys the GDILifetime instance and releases all GDI objects in the stack.
         *
         * This destructor automatically releases all GDI objects in the stack by calling their respective destroy methods.
         */
        virtual ~GDILifetime();

        /**
         * @brief Move an rvalue reference onto the stack.
         * 
         * @param object The rvalue reference to the GDIObjectInstance
         */
        void pushObject(const GDIObjectInstance&& object);

        /**
         * @brief Clears the stack of GDIObjectInstance instances.
         *
         * This method  releases all GDI objects in the stack by calling their respective destroy methods.
         */
        void clearStack();
    };

}