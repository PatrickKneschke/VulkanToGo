#pragma once


#include "vk_core.h"


namespace vktg
{


    vk::Fence CreateFence( vk::FenceCreateFlags flags = vk::FenceCreateFlagBits::eSignaled);
    
    void DestroyFence( vk::Fence fence);


    vk::Semaphore CreateSemaphore( vk::SemaphoreType type = vk::SemaphoreType::eBinary);
    
    void DestroySemaphore( vk::Semaphore semaphore);


    vk::MemoryBarrier2 CreateMemoryBarrier( 
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess, 
        vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess
    );

    vk::BufferMemoryBarrier2 CreateBufferMemoryBarrier(
        vk::Buffer buffer, 
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess, 
        vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess,
        size_t offset = 0, size_t size = VK_WHOLE_SIZE 
    );
    
    vk::ImageMemoryBarrier2 CreateImageMemoryBarrier(
        vk::Image image, 
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccessMask, 
        vk::PipelineStageFlags2 dststage, vk::AccessFlags2 dstAccessMask,
        vk::ImageSubresourceRange subResource = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} 
    );

    
} // namespace vktg
