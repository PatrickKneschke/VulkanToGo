#pragma once


#include "vk_core.h"

#include <span>


namespace vktg 
{


    vk::CommandPool CreateCommandPool(
        uint32_t queueFamily,
		vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer
    );
    
    void DestroyCommandPool( vk::CommandPool pool);


    vk::CommandBuffer AllocateCommandBuffer(
        vk::CommandPool commandPool,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary
    );


    void SubmitCommands( 
        vk::Queue queue, 
        std::span<vk::CommandBufferSubmitInfo> commandBuffers,
        std::span<vk::SemaphoreSubmitInfo> waitSemaphores, 
        std::span<vk::SemaphoreSubmitInfo> signalSemaphores,
        vk::Fence fence
    );

    
} // namespace vktg
