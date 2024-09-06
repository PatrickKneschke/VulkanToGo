
#define VMA_IMPLEMENTATION
#include "vk_core.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>


// unique definition of default dispatch loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


namespace vktg
{

    /***   CONFIG   ***/

    static std::string sWindowTitle = "Main";
    static uint32_t sWindowWidth = 1920;
    static uint32_t sWindowHeight = 1080;

    static std::vector<const char*> GetRequiredExtensions() {

        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME
        };
    }

    static vk::PhysicalDeviceFeatures2 GetVulkan10DeviceFeatures() {

        auto features = vk::PhysicalDeviceFeatures2{};
        features.features
            .setSamplerAnisotropy( VK_TRUE)
            .setSampleRateShading( VK_TRUE)
            .setDepthClamp( VK_TRUE)
            .setGeometryShader( VK_TRUE)
            .setTessellationShader( VK_TRUE)
            .setMultiDrawIndirect( VK_TRUE)
            .setDrawIndirectFirstInstance( VK_TRUE)
            .setFillModeNonSolid( VK_TRUE)
            .setShaderFloat64( VK_TRUE)
            .setWideLines( VK_TRUE);

        return features;
    }

    static vk::PhysicalDeviceVulkan11Features GetVulkan11DeviceFeatures() {
        
        auto features11 = vk::PhysicalDeviceVulkan11Features{};

        return features11;
    }

    static vk::PhysicalDeviceVulkan12Features GetVulkan12DeviceFeatures() {
        
        auto features12 = vk::PhysicalDeviceVulkan12Features{}
            .setBufferDeviceAddress( VK_TRUE )
            .setSamplerFilterMinmax( VK_TRUE )
            .setTimelineSemaphore( VK_TRUE )
            .setDescriptorIndexing( VK_TRUE );

        return features12;
    }

    static vk::PhysicalDeviceVulkan13Features GetVulkan13DeviceFeatures() {
        
        auto features13 = vk::PhysicalDeviceVulkan13Features{}
            .setDynamicRendering( VK_TRUE );

        return features13;
    }

    /***   CONFIG END  ***/


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
            window = glfwCreateWindow( sWindowWidth, sWindowHeight, sWindowTitle.c_str(), nullptr, nullptr);

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
            auto requiredExtensions = GetRequiredExtensions();

            // enable device features
            auto enabledFeatures = GetVulkan10DeviceFeatures();
            auto enabledFeatures11 = GetVulkan11DeviceFeatures();
            auto enabledFeatures12 = GetVulkan12DeviceFeatures();
            auto enabledFeatures13 = GetVulkan13DeviceFeatures();
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
            Queue( QueueType::GRAPHICS, graphicsQueueIdx);
            QueueIndex( QueueType::GRAPHICS, graphicsQueueIdx);
            if (computeQueueIdx >= 0)
            {
                Queue( QueueType::COMPUTE, computeQueueIdx);
                QueueIndex( QueueType::COMPUTE, computeQueueIdx);
            }
            else
            {
                Queue( QueueType::COMPUTE, graphicsQueueIdx);
                QueueIndex( QueueType::COMPUTE, graphicsQueueIdx);
            }
            if (transferQueueIdx >= 0)
            {
                Queue( QueueType::TRANSFER, transferQueueIdx);
                QueueIndex( QueueType::TRANSFER, transferQueueIdx);
            }
            else
            {
                Queue( QueueType::TRANSFER, graphicsQueueIdx);
                QueueIndex( QueueType::TRANSFER, graphicsQueueIdx);
            }
        }

        return device;
    }


    uint32_t QueueIndex( QueueType type, int queueIdx) {

        static int32_t graphicsQueueIdx = -1;
        static int32_t computeQueueIdx = -1;
        static int32_t transferQueueIdx = -1;

        uint32_t idx;
        if (type == QueueType::GRAPHICS)
        {
            if (graphicsQueueIdx < 0  &&  queueIdx >= 0)
            {
                graphicsQueueIdx = queueIdx;
            }
            idx = graphicsQueueIdx;
        }
        else if (type == QueueType::COMPUTE)
        {
            if (computeQueueIdx < 0  &&  queueIdx >= 0)
            {
                computeQueueIdx = queueIdx;
            }
            idx = computeQueueIdx;
        }
        else if (type == QueueType::TRANSFER)
        {
            if (transferQueueIdx < 0  &&  queueIdx >= 0)
            {
                transferQueueIdx = queueIdx;
            }
            idx = transferQueueIdx;
        }

        return idx;
    }

    uint32_t GraphicsQueueIndex() { return QueueIndex( QueueType::GRAPHICS); }
    uint32_t ComputeQueueIndex() { return QueueIndex( QueueType::COMPUTE); }
    uint32_t TransferQueueIndex() { return QueueIndex( QueueType::TRANSFER); }


    vk::Queue Queue( QueueType type, int queueIdx) {

        static vk::Queue graphicsQueue;
        static vk::Queue computeQueue;
        static vk::Queue transferQueue;

        vk::Queue queue;
        if (type == QueueType::GRAPHICS)
        {
            if (!graphicsQueue)
            {
                Device().getQueue( queueIdx, 0, &graphicsQueue);
            }
            queue = graphicsQueue;
        }
        else if (type == QueueType::COMPUTE)
        {
            if (!computeQueue)
            {
                Device().getQueue( queueIdx, 0, &computeQueue);
            }
            queue = computeQueue;
        }
        else if (type == QueueType::TRANSFER)
        {
            if (!transferQueue)
            {
                Device().getQueue( queueIdx, 0, &transferQueue);
            }
            queue = transferQueue;
        }

        return queue;
    }

    vk::Queue GraphicsQueue() { return Queue( QueueType::GRAPHICS); }
    vk::Queue ComputeQueue() { return Queue( QueueType::COMPUTE); }
    vk::Queue TransferQueue() { return Queue( QueueType::TRANSFER); }


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


} // namespace vktg
