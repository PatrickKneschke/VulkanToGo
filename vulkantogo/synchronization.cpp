
#include "synchronization.h"


namespace vktg
{


    vk::Fence CreateFence( vk::FenceCreateFlags flags) {

        auto fenceInfo = vk::FenceCreateInfo{}
            .setFlags( flags);

        vk::Fence fence;
        VK_CHECK( Device().createFence( &fenceInfo, nullptr, &fence) );

        return fence;
    }


    void DestroyFence( vk::Fence fence) {
    
        Device().destroyFence( fence);
    }

    
    vk::Semaphore CreateSemaphore( vk::SemaphoreType type) {

        auto semaphoreTypeInfo = vk::SemaphoreTypeCreateInfo{}
            .setSemaphoreType( type );

        auto semaphoreInfo = vk::SemaphoreCreateInfo{}
            .setPNext( &semaphoreTypeInfo );

        vk::Semaphore semaphore;    
        VK_CHECK( Device().createSemaphore( &semaphoreInfo, nullptr, &semaphore) );

        return semaphore;
    }


    void DestroySemaphore(vk::Semaphore semaphore) {
    
        Device().destroySemaphore( semaphore);
    }

    
    vk::MemoryBarrier2 CreateMemoryBarrier( vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess) {

        auto barrier = vk::MemoryBarrier2()
            .setSrcStageMask( srcStage )
            .setSrcAccessMask( srcAccess )
            .setDstStageMask( dstStage )
            .setDstAccessMask( dstAccess );

        return barrier;
    }

    
    vk::BufferMemoryBarrier2 CreateBufferMemoryBarrier( vk::Buffer buffer, vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccess, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess, size_t offset, size_t size) {

        auto barrier = vk::BufferMemoryBarrier2()
            .setBuffer( buffer )
            .setSrcStageMask( srcStage)
            .setSrcAccessMask( srcAccess )
            .setDstStageMask( dstStage )
            .setDstAccessMask( dstAccess )
            .setOffset( offset )
            .setSize( size )
            .setSrcQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
            .setDstQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED );

        return barrier;
    }


    vk::ImageMemoryBarrier2 CreateImageMemoryBarrier( vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dststage, vk::AccessFlags2 dstAccessMask, vk::ImageSubresourceRange subResource) {

        auto barrier = vk::ImageMemoryBarrier2{}
            .setImage( image )
            .setOldLayout( oldLayout ) 
            .setNewLayout( newLayout )
            .setSrcStageMask( srcStage )
            .setSrcAccessMask( srcAccessMask )
            .setDstStageMask( dststage )
            .setDstAccessMask( dstAccessMask )
            .setSubresourceRange( subResource )
            .setSrcQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
            .setDstQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED );

        return barrier;
    }


} // namespace vktg