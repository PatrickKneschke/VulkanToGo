#pragma once


#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "vma/vk_mem_alloc.hpp"


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
    
    ConfigSettings* Config();
    GLFWwindow* Window();
    vk::Instance Instance();
    vk::DebugUtilsMessengerEXT DebugMessenger();
    vk::SurfaceKHR Surface();
    vk::PhysicalDevice Gpu();
    vk::Device Device();
    uint32_t QueueIndex( QueueType type, int queueIdx = -1);
    uint32_t GraphicsQueueIndex();
    uint32_t ComputeQueueIndex();
    uint32_t TransferQueueIndex();
    vk::Queue Queue( QueueType type, int queueIdx = -1);
    vk::Queue GraphicsQueue();
    vk::Queue ComputeQueue();
    vk::Queue TransferQueue();
    vma::Allocator Allocator();

    void StartUp();
    void ShutDown();
    void WaitIdle();


} // namespace vktg
