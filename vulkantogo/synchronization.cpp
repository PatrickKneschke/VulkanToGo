
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


    void WaitForFence( vk::Fence fence, uint64_t timeout) {

        VK_CHECK( vktg::Device().waitForFences( 1, &fence, true, timeout) );
    }

    
    void WaitForFences( std::span<vk::Fence> fences, bool waitAll, uint64_t timeout) {

        VK_CHECK( vktg::Device().waitForFences( fences.size(), fences.data(), waitAll, timeout) );
    }


    void ResetFence( vk::Fence fence) {

        VK_CHECK( vktg::Device().resetFences( 1, &fence) );
    }


    void ResetFences( std::span<vk::Fence> fences) {

        VK_CHECK( vktg::Device().resetFences( fences.size(), fences.data()) );
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

    
    void SignalSemaphore(vk::Semaphore semaphore, uint64_t value) {

        auto signalInfo = vk::SemaphoreSignalInfo{}
            .setSemaphore( semaphore )
            .setValue( value );

        VK_CHECK( vktg::Device().signalSemaphore( &signalInfo) );
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