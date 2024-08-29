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
    

    GLFWwindow* Window();
    vk::Instance Instance();
    vk::DebugUtilsMessengerEXT DebugMessenger();
    vk::SurfaceKHR Surface();
    vk::PhysicalDevice Gpu();
    vk::Device Device();


    void StartUp();
    void ShutDown();


} // namespace vktg
