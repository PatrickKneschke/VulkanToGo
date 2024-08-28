
#define VMA_IMPLEMENTATION
#include "vk_core.h"

#include <vector>
#include <iostream>


// unique definition of default dispatch loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


namespace vktg
{


    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*  )  
    {
        std::cerr << "validation layer  [" <<  messageSeverity << "], (" << messageType << ") : " << pCallbackData->pMessage << '\n';

        return VK_FALSE;
    }


    GLFWwindow *Window() {

        static GLFWwindow* window;

        if (!window)
        {
            // init glfw
            if (!glfwInit())
            {
                std::cerr << "Failed to init glfw.";
            }

            // configure GLFW window
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // for vulkan
            
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            glfwWindowHint(GLFW_RED_BITS, 8);
            glfwWindowHint(GLFW_GREEN_BITS, 8);
            glfwWindowHint(GLFW_BLUE_BITS, 8);
            glfwWindowHint(GLFW_ALPHA_BITS, 8);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);

            // create window
            window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

            // additional window configs
            // glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        return window;
    }


    vk::Instance Instance() {

        static vk::Instance instance;

        if ( !instance)
        {
            // default dispatch loader dynamic
            vk::DynamicLoader dl;
            PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
            VULKAN_HPP_DEFAULT_DISPATCHER.init( vkGetInstanceProcAddr);

            auto appInfo = vk::ApplicationInfo{}
                .setPEngineName( "No Engine")
                .setEngineVersion(  VK_MAKE_VERSION(1, 0, 0) )
                .setApiVersion( VK_API_VERSION_1_3 );

            uint32_t glfwExtensionCount = 0;
            auto glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount);
            std::vector<const char*> glfwExtensionsVector( glfwExtensions, glfwExtensions + glfwExtensionCount);
            std::vector<const char*> layers;

#ifndef NDEBUG
            glfwExtensionsVector.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            layers.push_back( "VK_LAYER_KHRONOS_validation");
#endif

            auto instanceInfo = vk::InstanceCreateInfo{}
                .setPApplicationInfo( &appInfo )
                .setEnabledExtensionCount( static_cast<uint32_t>( glfwExtensionsVector.size()) )
                .setPpEnabledExtensionNames( glfwExtensionsVector.data() )
                .setEnabledLayerCount( static_cast<uint32_t>( layers.size()) )
                .setPpEnabledLayerNames( layers.data() );
            
            VK_CHECK( vk::createInstance( &instanceInfo, nullptr, &instance) );
            
            // initialize dispatch loader with instance, if creation was successful
            VULKAN_HPP_DEFAULT_DISPATCHER.init( instance);
        }

        return instance;
    }


    vk::DebugUtilsMessengerEXT DebugMessenger() {

        static vk::DebugUtilsMessengerEXT debugMessenger;

        if (!debugMessenger)
        {
            auto debugMessengerInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
                .setMessageSeverity(
                    // vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError 
                )
                .setMessageType(
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance 
                )
                .setPfnUserCallback( DebugCallback);	
            
            VK_CHECK( Instance().createDebugUtilsMessengerEXT( &debugMessengerInfo, nullptr, &debugMessenger) );
        }

        return debugMessenger;
    }


    vk::SurfaceKHR Surface() {

        static vk::SurfaceKHR surface;

        if (!surface)
        {
            VkSurfaceKHR tempSurface;
            VK_CHECK( (vk::Result)glfwCreateWindowSurface( Instance(), Window(), nullptr, &tempSurface) );
            surface = vk::SurfaceKHR( tempSurface);
        }

        return surface;
    }


    vk::PhysicalDevice Gpu() {

        static vk::PhysicalDevice chosenGpu;

        if (!chosenGpu)
        {
            auto gpus = Instance().enumeratePhysicalDevices();
            for (auto &gpu : gpus) 
            {	
                auto properties = gpu.getProperties();

                if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                {
                    chosenGpu = gpu;
                    break;
                }
                else if ( properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
                {
                    chosenGpu = gpu;
                }
            }

            if( !chosenGpu)
            {
                throw std::runtime_error("Failed to find suitable GPU!");
            }
        }

        return chosenGpu;
    }


    void StartUp() {

        Window();
        Instance();
#ifndef NDEBUG
        DebugMessenger();
#endif
        Surface();
        Gpu();
    }


    void ShutDown() {

        // surface
        Instance().destroySurfaceKHR( Surface());

        // debug messenger
#ifndef NDEBUG
        Instance().destroyDebugUtilsMessengerEXT( DebugMessenger());
#endif

        // instance
        Instance().destroy();

        // window
        glfwDestroyWindow( Window());
        glfwTerminate();
    }


} // namespace vktg
