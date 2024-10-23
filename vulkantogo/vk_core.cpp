
#define VMA_IMPLEMENTATION
#include "vk_core.h"

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>


// unique definition of default dispatch loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


namespace vktg
{

    /***    DEFAULT CONFIG    ***/

    static void SetRequiredExtensionsDefault( std::vector<const char*>& extensions ) {

        extensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        extensions.push_back( VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME );
        extensions.push_back( VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME );
    }

    static void ConfigureGlfwDefault() {

        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API); // for vulkan
        
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);

        glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE);
    }

    static void ConfigureGlfwWindowDefault(GLFWwindow *window) {

    }

    static void SetVulkan10DeviceFeaturesDefault( vk::PhysicalDeviceFeatures2& features) {

        features.features
            .setFullDrawIndexUint32( VK_TRUE )
            .setSampleRateShading( VK_TRUE )
            .setMultiDrawIndirect( VK_TRUE )
            .setDrawIndirectFirstInstance( VK_TRUE )
            .setDepthBounds( VK_TRUE )
            .setDepthClamp( VK_TRUE )
            .setDepthBiasClamp( VK_TRUE )
            .setFillModeNonSolid( VK_TRUE )
            .setLargePoints( VK_TRUE )
            .setWideLines( VK_TRUE )
            .setSamplerAnisotropy( VK_TRUE )
            .setShaderFloat64( VK_TRUE);
    }

    static void SetVulkan11DeviceFeaturesDefault( vk::PhysicalDeviceVulkan11Features& features) {
        
    }

    static void SetVulkan12DeviceFeaturesDefault( vk::PhysicalDeviceVulkan12Features& features) {
        
        features
            .setSamplerMirrorClampToEdge( VK_TRUE )
            .setDrawIndirectCount( VK_TRUE )
            .setSamplerFilterMinmax( VK_TRUE )
            .setBufferDeviceAddress( VK_TRUE )
            .setTimelineSemaphore( VK_TRUE );
    }

    static void SetVulkan13DeviceFeaturesDefault( vk::PhysicalDeviceVulkan13Features& features) {
        
        features
            .setSynchronization2( VK_TRUE )
            .setDynamicRendering( VK_TRUE );
    }

    /***    DEFAULT CONFIG END    ***/


    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*  )  
    {
        Config()->debugCallback( messageSeverity, messageType, pCallbackData);

        return VK_FALSE;
    }


    ConfigSettings* Config() {

        static ConfigSettings *pConfig;

        if (!pConfig)
        {
            pConfig = new ConfigSettings();
            pConfig->windowTitle = "Main";
            pConfig->windowWidth = 1920;
            pConfig->windowHeight = 1080;
            pConfig->fullScreen = false;
            pConfig->debugCallback = [](
                VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT             messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
            {
                std::cerr << "validation layer  [" <<  messageSeverity << "], (" << messageType << ") : " << pCallbackData->pMessage << '\n';
            };

        }

        return pConfig;
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
            ConfigureGlfwDefault();
            if (Config()->configureGlfw)
            {
                Config()->configureGlfw();
            }

            // create window
            if (Config()->fullScreen)
            {
                auto monitor = glfwGetPrimaryMonitor();
                auto mode = glfwGetVideoMode( monitor);
                window = glfwCreateWindow( mode->width, mode->height, Config()->windowTitle.c_str(), monitor, nullptr);
            }
            else 
            {
                window = glfwCreateWindow( Config()->windowWidth, Config()->windowHeight, Config()->windowTitle.c_str(), nullptr, nullptr);
            }

            // additional window configs
            if (Config()->configureGlfwWindow)
            {
                Config()->configureGlfwWindow( window);
            }
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
                .setPfnUserCallback( DebugCallback );	
            
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

            if (!chosenGpu)
            {
                throw std::runtime_error("Failed to find suitable GPU!");
            }
        }

        return chosenGpu;
    }

    
    vk::Device Device() {

        static vk::Device device;

        if (!device)
        {
            // find queue family indices
            // pick seperate queues for compute and transfer commands if available
            int graphicsQueueIdx = -1, computeQueueIdx = -1, transferQueueIdx = -1;
            auto queueFamilies = Gpu().getQueueFamilyProperties();
            for (int i = 0; i < queueFamilies.size(); i++)
            {
                if ( queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    graphicsQueueIdx = i;
                }
                else if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)
                {
                    computeQueueIdx = i;
                }
                else if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)
                {
                    transferQueueIdx = i;
                }
            }
            std::unordered_set<int> uniqueQueueFamilies = { graphicsQueueIdx, computeQueueIdx, transferQueueIdx};

            // queue create infos
            std::vector<vk::DeviceQueueCreateInfo> queueInfos;
            float priority = 0.f;
            for (auto family : uniqueQueueFamilies)
            {
                if (family < 0)
                {
                    continue;
                }

                auto queueInfo = vk::DeviceQueueCreateInfo{}
                    .setPQueuePriorities( &priority )
                    .setQueueCount( 1 )
                    .setQueueFamilyIndex( family );

                queueInfos.push_back( queueInfo);
            }

            // required extentions
            std::vector<const char*> requiredExtensions;
            SetRequiredExtensionsDefault( requiredExtensions);    
            if (Config()->setRequiredExtensions)
            {
                Config()->setRequiredExtensions( requiredExtensions);
            }

            // enable device features
            auto enabledFeatures = vk::PhysicalDeviceFeatures2{};
            SetVulkan10DeviceFeaturesDefault( enabledFeatures);
            if (Config()->setVulkan10DeviceFeatures)
            {
                Config()->setVulkan10DeviceFeatures( enabledFeatures);
            }            
            auto enabledFeatures11 = vk::PhysicalDeviceVulkan11Features{};
            SetVulkan11DeviceFeaturesDefault( enabledFeatures11);
            if (Config()->setVulkan11DeviceFeatures)
            {
                Config()->setVulkan11DeviceFeatures( enabledFeatures11);
            }
            auto enabledFeatures12 = vk::PhysicalDeviceVulkan12Features{};
            SetVulkan12DeviceFeaturesDefault( enabledFeatures12);
            if (Config()->setVulkan12DeviceFeatures)
            {
                Config()->setVulkan12DeviceFeatures( enabledFeatures12);
            }
            auto enabledFeatures13 = vk::PhysicalDeviceVulkan13Features{};
            SetVulkan13DeviceFeaturesDefault( enabledFeatures13);
            if (Config()->setVulkan13DeviceFeatures)
            {
                Config()->setVulkan13DeviceFeatures( enabledFeatures13);
            }
            enabledFeatures.pNext = &enabledFeatures11;
            enabledFeatures11.pNext = &enabledFeatures12;
            enabledFeatures12.pNext = &enabledFeatures13;

            // create device
            auto deviceInfo = vk::DeviceCreateInfo{}
                .setQueueCreateInfoCount( (uint32_t)queueInfos.size() )
                .setPQueueCreateInfos( queueInfos.data() )
                .setEnabledExtensionCount( (uint32_t)requiredExtensions.size() )
                .setPpEnabledExtensionNames( requiredExtensions.data() )
                .setPEnabledFeatures( nullptr )
                .setPNext( &enabledFeatures);

            VK_CHECK( Gpu().createDevice(&deviceInfo, nullptr, &device) );

            // init dispatch loader with device
            VULKAN_HPP_DEFAULT_DISPATCHER.init( device);

            // create queues
            Queue( QueueType::eGraphics, graphicsQueueIdx);
            QueueIndex( QueueType::eGraphics, graphicsQueueIdx);
            if (computeQueueIdx >= 0)
            {
                Queue( QueueType::eCompute, computeQueueIdx);
                QueueIndex( QueueType::eCompute, computeQueueIdx);
            }
            else
            {
                Queue( QueueType::eCompute, graphicsQueueIdx);
                QueueIndex( QueueType::eCompute, graphicsQueueIdx);
            }
            if (transferQueueIdx >= 0)
            {
                Queue( QueueType::eTransfer, transferQueueIdx);
                QueueIndex( QueueType::eTransfer, transferQueueIdx);
            }
            else
            {
                Queue( QueueType::eTransfer, graphicsQueueIdx);
                QueueIndex( QueueType::eTransfer, graphicsQueueIdx);
            }
        }

        return device;
    }


    uint32_t QueueIndex( QueueType type, int queueIdx) {

        static int32_t graphicsQueueIdx = -1;
        static int32_t computeQueueIdx = -1;
        static int32_t transferQueueIdx = -1;

        uint32_t idx;
        if (type == QueueType::eGraphics)
        {
            if (graphicsQueueIdx < 0  &&  queueIdx >= 0)
            {
                graphicsQueueIdx = queueIdx;
            }
            idx = graphicsQueueIdx;
        }
        else if (type == QueueType::eCompute)
        {
            if (computeQueueIdx < 0  &&  queueIdx >= 0)
            {
                computeQueueIdx = queueIdx;
            }
            idx = computeQueueIdx;
        }
        else if (type == QueueType::eTransfer)
        {
            if (transferQueueIdx < 0  &&  queueIdx >= 0)
            {
                transferQueueIdx = queueIdx;
            }
            idx = transferQueueIdx;
        }

        return idx;
    }

    uint32_t GraphicsQueueIndex() { return QueueIndex( QueueType::eGraphics); }
    uint32_t ComputeQueueIndex() { return QueueIndex( QueueType::eCompute); }
    uint32_t TransferQueueIndex() { return QueueIndex( QueueType::eTransfer); }


    vk::Queue Queue( QueueType type, int queueIdx) {

        static vk::Queue graphicsQueue;
        static vk::Queue computeQueue;
        static vk::Queue transferQueue;

        vk::Queue queue;
        if (type == QueueType::eGraphics)
        {
            if (!graphicsQueue)
            {
                Device().getQueue( queueIdx, 0, &graphicsQueue);
            }
            queue = graphicsQueue;
        }
        else if (type == QueueType::eCompute)
        {
            if (!computeQueue)
            {
                Device().getQueue( queueIdx, 0, &computeQueue);
            }
            queue = computeQueue;
        }
        else if (type == QueueType::eTransfer)
        {
            if (!transferQueue)
            {
                Device().getQueue( queueIdx, 0, &transferQueue);
            }
            queue = transferQueue;
        }

        return queue;
    }

    vk::Queue GraphicsQueue() { return Queue( QueueType::eGraphics); }
    vk::Queue ComputeQueue() { return Queue( QueueType::eCompute); }
    vk::Queue TransferQueue() { return Queue( QueueType::eTransfer); }


    vma::Allocator Allocator() {

        static vma::Allocator allocator;

        if ( !allocator)
        {
            auto allocatorInfo = vma::AllocatorCreateInfo{}
                .setInstance( Instance() )
                .setPhysicalDevice( Gpu() )
                .setDevice( Device() )
                .setFlags( vma::AllocatorCreateFlagBits::eBufferDeviceAddress);
                
            VK_CHECK( vma::createAllocator( &allocatorInfo, &allocator) );
        }

        return allocator;
    }


    void StartUp() {

        Window();
        Instance();
#ifndef NDEBUG
        DebugMessenger();
#endif
        Surface();
        Gpu();
        Device();
        Allocator();
    }


    void ShutDown() {

        // allocator
        Allocator().destroy();

        // device
        Device().destroy();

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

    
    void WaitIdle() {

        Device().waitIdle();
    }


} // namespace vktg
