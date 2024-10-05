
#include "submit_context.h"
#include "commands.h"
#include "synchronization.h"


namespace vktg
{


    SubmitContext CreateSubmitContext( vktg::QueueType queueType) {

        SubmitContext context;
        context.queue = Queue( queueType);
        context.cmdPool = CreateCommandPool( QueueIndex( queueType));
        context.cmd = AllocateCommandBuffer( context.cmdPool);
        context.fence = CreateFence( vk::FenceCreateFlags{});

        return context;
    }

    
    void DestroySubmitContext( const SubmitContext &context) {

        DestroyCommandPool( context.cmdPool);
        DestroyFence( context.fence);
    }

    
    void SubmitContext::Begin() {

        VK_CHECK( Device().resetFences( 1, &fence) );

        cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);
        auto cmdBeginInfo = vk::CommandBufferBeginInfo{}
            .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );		
        cmd.begin( cmdBeginInfo);
    }

    
    void SubmitContext::End() {

        cmd.end();
    }

    
    void SubmitContext::Submit() {

        auto submitInfo = vk::SubmitInfo{}
            .setCommandBufferCount( 1 )
            .setPCommandBuffers( &cmd );

        VK_CHECK( queue.submit( 1, &submitInfo, fence) );
    }


} // namespace vktg