#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    /// @brief Creates Vulkan Fence.
    /// @param flags Fence creation flags.
    /// @return Vulakn Fence.
    vk::Fence CreateFence( vk::FenceCreateFlags flags = vk::FenceCreateFlagBits::eSignaled);
    
    /// @brief Destroys given Vulkan fence.
    /// @param fence Fence to destroy.
    void DestroyFence( vk::Fence fence);

    /// @brief Waits for a single Vulkan fence to be signaled.
    /// @param fence Fecne to wait on.
    /// @param timeout Timeout value in nanoseconds. Will throw if waiting takes longer than this value.
    void WaitForFence( vk::Fence fence, uint64_t timeout = 1e9);
    /// @brief Waits for multiple Vulkan fences to be signaled.
    /// @param fences List of fences to wait on.
    /// @param waitAll Wether to wait for all fences or just one of them to be signaled.
    /// @param timeout Timeout value in nanoseconds. Will throw if waiting takes longer than this value.
    void WaitForFences( std::span<vk::Fence> fences, bool waitAll = true, uint64_t timeout = 1e9);

    /// @brief Resets single Vulkan fence to unsignaled state.
    /// @param fence Fence to reset.
    void ResetFence( vk::Fence fence);
    /// @brief Resets multiple Vulkan fences to unsignaled state.
    /// @param fences List of fences to reset.
    void ResetFences( std::span<vk::Fence> fences);


    /// @brief Creates Vulkan semaphore.
    /// @param type Semaphore type.
    /// @return Vulkan semaphore.
    vk::Semaphore CreateSemaphore( vk::SemaphoreType type = vk::SemaphoreType::eBinary);
    
    /// @brief Destroys given Vulkan semaphore.
    /// @param semaphore Semaphore to destroy
    void DestroySemaphore( vk::Semaphore semaphore);

    /// @brief Signals Vulkan timeline semaphore.
    /// @param semaphore Timeline semaphore to signal.
    /// @param value Value to assign to the semaphores counter value.
    void SignalSemaphore( vk::Semaphore semaphore, uint64_t value);


    /// @brief Creates Vulkan memory barrier.
    /// @param srcStage Source pipeline stage.
    /// @param srcAccess Source memory access.
    /// @param dstStage Destination pipeline stage.
    /// @param dstAccess Destination memory access.
    /// @return Vulkan memory barrier.
    vk::MemoryBarrier2 CreateMemoryBarrier( 
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess, 
        vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess
    );

    /// @brief Creates Vulkan buffer memory barrier.
    /// @param buffer Buffer associated with the barrier.
    /// @param srcStage Source pipeline stage.
    /// @param srcAccess Source memory access.
    /// @param dstStage Destination pipeline stage.
    /// @param dstAccess Destination memory access.
    /// @param offset Offset of the buffer regon the barrier applies to.
    /// @param size Size of the buffer region the barrier applies to.
    /// @return Vulkan buffer memory barrier.
    vk::BufferMemoryBarrier2 CreateBufferMemoryBarrier(
        vk::Buffer buffer, 
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess, 
        vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess,
        size_t offset = 0, size_t size = VK_WHOLE_SIZE 
    );
    
    /// @brief Creates Vulkan image memory barrier.
    /// @param image Image associated with the barrier.
    /// @param oldLayout Old image layout.
    /// @param newLayout New image layout.
    /// @param srcStage Source pipeline stage.
    /// @param srcAccess Source memory access.
    /// @param dstStage Destination pipeline stage.
    /// @param dstAccess Destination memory access.
    /// @param subResource Image subresource the barrier applies to.
    /// @return 
    vk::ImageMemoryBarrier2 CreateImageMemoryBarrier(
        vk::Image image, 
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccessMask, 
        vk::PipelineStageFlags2 dststage, vk::AccessFlags2 dstAccessMask,
        const vk::ImageSubresourceRange &subResource = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} 
    );

    
} // namespace vktg
