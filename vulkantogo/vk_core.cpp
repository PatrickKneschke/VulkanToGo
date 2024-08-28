
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


    void StartUp(uint32_t windowWidth, uint32_t windowHeight, std::string windowTitle) {

        Instance();
#ifndef NDEBUG
        DebugMessenger();
#endif
    }


} // namespace vktg
