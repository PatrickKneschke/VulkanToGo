
#include "vulkantogo.h"

#include <iostream>


int main() {

    vktg::StartUp();


    // deletion stack
    vktg::DeletionStack deletionStack = vktg::DeletionStack{};


    // swapchain
    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    vktg::Image renderImage = vktg::CreateImage(
        swapchain.extent.width, swapchain.extent.height, vk::Format::eR16G16B16A16Sfloat, 
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
    ); 


    // synchronization objects
    vk::Fence renderFence = vktg::CreateFence();
    vk::Semaphore renderSemaphore = vktg::CreateSemaphore();
    vk::Semaphore presentSemaphore = vktg::CreateSemaphore();

    deletionStack.Push( [=](){
        vktg::DestroyFence( renderFence);
    });
    deletionStack.Push( [=](){
        vktg::DestroySemaphore( renderSemaphore);
    });
    deletionStack.Push( [=](){
        vktg::DestroySemaphore( presentSemaphore);
    });

    
    // command pool and command buffer
    vk::CommandPool graphicsCmdPool = vktg::CreateCommandPool( vktg::GraphicsQueueIndex());
    deletionStack.Push( [=](){
        vktg::DestroyCommandPool( graphicsCmdPool);
    });
    vk::CommandBuffer cmd = vktg::AllocateCommandBuffer( graphicsCmdPool);


    // render loop
    while (!glfwWindowShouldClose( vktg::Window())) 
    {
        // recreate swapchain if outdated
        if (!swapchain.isValid)
        {
            vktg::CreateSwapchain( swapchain);
        }

        // wait for render fence to record render commands
        VK_CHECK( vktg::Device().waitForFences( renderFence, true, 1e9) );
        VK_CHECK( vktg::Device().resetFences( 1, &renderFence) );


        // get next swapchain image
        uint32_t imageIndex;
        if (vktg::NextSwapchainImage( swapchain, renderSemaphore, &imageIndex))
        {
            continue;
        }


        // record render commands
        cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);
        auto commandBeginInfo = vk::CommandBufferBeginInfo{}
            .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
        VK_CHECK( cmd.begin( &commandBeginInfo) );

        cmd.end();


        vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        auto submitInfo = vk::SubmitInfo{}
            .setWaitSemaphoreCount( 1 )
            .setPWaitSemaphores( &renderSemaphore )
            .setPWaitDstStageMask( &waitStage )
            .setCommandBufferCount( 1 )
            .setPCommandBuffers( &cmd )
            .setSignalSemaphoreCount( 1 )
            .setPSignalSemaphores( &presentSemaphore );		
        VK_CHECK( vktg::GraphicsQueue().submit( 1, &submitInfo, renderFence) );

        vktg::PresentImage( swapchain, &presentSemaphore, &imageIndex);
    }


    // cleanup
    deletionStack.Flush();
    vktg::DestroySwapchain( swapchain);

    vktg::ShutDown();

    return 0;
}