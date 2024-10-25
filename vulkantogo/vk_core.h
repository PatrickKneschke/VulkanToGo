#pragma once


#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "vma/vk_mem_alloc.hpp"


/// @brief 
#define VK_CHECK( result)                                                                           \
    do                                                                                              \
    {                                                                                               \
        if (result != vk::Result::eSuccess)                                                         \
        {                                                                                           \
            throw std::runtime_error( "Vulkan Error : " + vk::to_string( result));                  \
        }                                                                                           \
    } while (0) 


namespace vktg
{

    /// @brief Holds user defined configurations for GLFW and Vulkan.
    struct ConfigSettings {

        // glfw window config
        std::string windowTitle;
        uint32_t windowWidth;
        uint32_t windowHeight;
        bool fullScreen;
        std::function<void()> configureGlfw;
        std::function<void(GLFWwindow*)> configureGlfwWindow;

        // vulkan required device extentions
        std::function<void(std::vector<const char*>&)> setRequiredExtensions;

        // vulkan device features
        std::function<void(vk::PhysicalDeviceFeatures2&)> setVulkan10DeviceFeatures;
        std::function<void(vk::PhysicalDeviceVulkan11Features&)> setVulkan11DeviceFeatures;
        std::function<void(vk::PhysicalDeviceVulkan12Features&)> setVulkan12DeviceFeatures;
        std::function<void(vk::PhysicalDeviceVulkan13Features&)> setVulkan13DeviceFeatures;

        // vulkan debug callback
        std::function<void(
            VkDebugUtilsMessageSeverityFlagBitsEXT, 
            VkDebugUtilsMessageTypeFlagsEXT,
            const VkDebugUtilsMessengerCallbackDataEXT*
            )> debugCallback;
    };


    enum class QueueType : uint8_t {
        eGraphics = 0,
        eCompute = 1,
        eTransfer = 2
    };
    

    /// @brief Access GLFW window. Creates window with specified configuration upon first call.
    /// @return Pointer to the created glfw window.
    GLFWwindow* Window();
    /// @brief Access Vulkan instance. Creates instance upon first call.
    /// @return Vulkan instance.
    vk::Instance Instance();
    /// @brief Access Vulkan debug messenger, Creates debug mesenger upon first call. 
    /// @return Vulkan debug messenger.
    vk::DebugUtilsMessengerEXT DebugMessenger();
    /// @brief Access Vulkan surface, Creates surface upon first call.
    /// @return Vulkan surface.
    vk::SurfaceKHR Surface();
    /// @brief Access Vulkan physical device. Selects suitable physical device upon first call.
    /// @return Vulkan physical device.
    vk::PhysicalDevice Gpu();
    /// @brief Access Vulkan device. Creates device with specified configuration upon frst call.
    /// @return Vulkan device. 
    vk::Device Device();
    /// @brief Access Vulkan queue family index for specified queue type. The first call providing a queueIdx will assign this index to queues of that type.
    ///        Intended for internal use only. Access specific queue indices with the dedicated *QueueIndex() functions for graphics, compute and transfer.
    /// @param type Type of queue.
    /// @param queueIdx Queue family index for this queue type.
    /// @return Vulkan queue family index.
    uint32_t QueueIndex( QueueType type, int queueIdx = -1);
    /// @brief Access Vulkan graphics queue family index.
    /// @return Vulkan graphics queue family index.
    uint32_t GraphicsQueueIndex();
    /// @brief Access Vulkan compute queue family index.
    /// @return Vulkan compute queue family index.
    uint32_t ComputeQueueIndex();
    /// @brief Access Vulkan transfer queue family index.
    /// @return Vulkan transfer queue family index.
    uint32_t TransferQueueIndex();
    /// @brief Access Vulkan queue for specified queue type. The first call providing a queueIdx will create the queue from the queue family of that index.
    ///        Intended for internal use only. Access specific queues with the dedicated *Queue() functions for graphics, compute and transfer.
    /// @param type Type of queue.
    /// @param queueIdx Queue family index for this queue type.
    /// @return Vulkan queue.
    vk::Queue Queue( QueueType type, int queueIdx = -1);
    /// @brief Access Vulkan graphics queue.
    /// @return Vulkan graphics queue.
    vk::Queue GraphicsQueue();
    /// @brief Access Vulkan compute queue.
    /// @return Vulkan compute queue.
    vk::Queue ComputeQueue();
    /// @brief Access Vulkan transfer queue.
    /// @return Vulkan transfer queue.
    vk::Queue TransferQueue();
    /// @brief Access Vulkan memory allocator. Creates allocator upon first call.
    /// @return Vulkan memory allocator.
    vma::Allocator Allocator();


    /// @brief Access and modify configurations for GLFW and Vulkan. Creates ConfigSettings with reasonable defaults upon first call.
    ///        Any customization must happen before call to StartUp().
    /// @return Pointer to static ConfigSettings object.
    ConfigSettings* Config();
    /// @brief Start up VulkanToGo. Creates glfw window, vulkan instance and surface. Selects gpu to use and creates Vulkan device, queues and memory allocator.
    ///        This is the first function you call before using the API and after setting user configuration.
    void StartUp();
    /// @brief Shut down VulkanToGo. Destroys Vulkan instance, device, surface, memory allocator and GLFW window.
    ///        This is the last function you call after you are done.
    void ShutDown();
    /// @brief Just a wrapper for vk::Device::waitIdle().
    void WaitIdle();


} // namespace vktg
