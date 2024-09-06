#pragma once


#include "vk_core.h"


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

    
} // namespace vktg
