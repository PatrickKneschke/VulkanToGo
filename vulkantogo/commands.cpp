

#include "commands.h"


namespace vktg 
{


    vk::CommandPool CreateCommandPool(
        uint32_t queueFamily,
        vk::CommandPoolCreateFlags flags) 
    {
        auto commandPoolInfo = vk::CommandPoolCreateInfo{}
            .setQueueFamilyIndex( queueFamily )
            .setFlags( flags );
            
        vk::CommandPool commandPool;
        VK_CHECK( Device().createCommandPool( &commandPoolInfo, nullptr, &commandPool) );
        
        return commandPool;
    }


    vk::CommandBuffer AllocateCommandBuffer(
        vk::CommandPool commandPool,
        vk::CommandBufferLevel level)
    {
        auto allocInfo = vk::CommandBufferAllocateInfo{}
            .setCommandPool( commandPool )
            .setCommandBufferCount( 1 )
            .setLevel( level );

        vk::CommandBuffer commandBuffer;
        VK_CHECK( Device().allocateCommandBuffers( &allocInfo, &commandBuffer) );
        
        return commandBuffer; 
    }


    void DestroyCommandPool( vk::CommandPool pool) {
    
        Device().destroyCommandPool( pool);
    }

    
} // namespace vktg
