#pragma once


#include "vk_core.h"

#include <span>


namespace vktg 
{


    /// @brief 
    /// @param queueFamily Queue family index to allocate command pool from.
    /// @param flags Command pool creation flags. 
    /// @return Vulkan command pool.
    vk::CommandPool CreateCommandPool(
        uint32_t queueFamily,
		vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer
    );
    
    /// @brief Destroy specified command pool.
    /// @param pool Command pool to destroy.
    void DestroyCommandPool( vk::CommandPool pool);


    /// @brief Allocates a new command buffer from a given command pool.
    /// @param commandPool Command pool to allocate command buffer from.
    /// @param level Primary or secondary command buffer.
    /// @return Allocated Vulkan command buffer.
    vk::CommandBuffer AllocateCommandBuffer(
        vk::CommandPool commandPool,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary
    );


    /// @brief Submit recorded commands to a given queue.
    /// @param queue Vulkan queue to submit commands to.
    /// @param commandBuffers Recorded command buffer.
    /// @param waitSemaphores List of submit infos for semaphores to wait on.
    /// @param signalSemaphores List of submit infos for semaphores to signal.
    /// @param fence Fence to signal upon completion.
    void SubmitCommands( 
        vk::Queue queue, 
        std::span<vk::CommandBufferSubmitInfo> commandBuffers,
        std::span<vk::SemaphoreSubmitInfo> waitSemaphores, 
        std::span<vk::SemaphoreSubmitInfo> signalSemaphores,
        vk::Fence fence
    );

    
} // namespace vktg
