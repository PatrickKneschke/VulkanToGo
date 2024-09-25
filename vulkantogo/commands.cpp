

#include "commands.h"


namespace vktg 
{


    vk::CommandPool CreateCommandPool( uint32_t queueFamily, vk::CommandPoolCreateFlags flags) {

        auto commandPoolInfo = vk::CommandPoolCreateInfo{}
            .setQueueFamilyIndex( queueFamily )
            .setFlags( flags );
            
        vk::CommandPool commandPool;
        VK_CHECK( Device().createCommandPool( &commandPoolInfo, nullptr, &commandPool) );
        
        return commandPool;
    }


    void DestroyCommandPool( vk::CommandPool pool) {
    
        Device().destroyCommandPool( pool);
    }


    vk::CommandBuffer AllocateCommandBuffer( vk::CommandPool commandPool, vk::CommandBufferLevel level) {

        auto allocInfo = vk::CommandBufferAllocateInfo{}
            .setCommandPool( commandPool )
            .setCommandBufferCount( 1 )
            .setLevel( level );

        vk::CommandBuffer commandBuffer;
        VK_CHECK( Device().allocateCommandBuffers( &allocInfo, &commandBuffer) );
        
        return commandBuffer; 
    }


    void SubmitCommands( vk::Queue queue, std::span<vk::CommandBufferSubmitInfo> commandBuffers, std::span<vk::SemaphoreSubmitInfo> waitSemaphores, std::span<vk::SemaphoreSubmitInfo> signalSemaphores, vk::Fence fence) {

        auto submitInfo = vk::SubmitInfo2{}
            .setCommandBufferInfoCount( commandBuffers.size() )
            .setPCommandBufferInfos( commandBuffers.data() )
            .setWaitSemaphoreInfoCount( waitSemaphores.size() )
            .setPWaitSemaphoreInfos( waitSemaphores.data() )
            .setSignalSemaphoreInfoCount( signalSemaphores.size() )
            .setPSignalSemaphoreInfos( signalSemaphores.data() );

        VK_CHECK( queue.submit2( 1, &submitInfo, fence) );
    }


} // namespace vktg
