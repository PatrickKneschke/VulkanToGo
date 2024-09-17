
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


    // descriptors
    uint32_t maxSetsPerPool = 10;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, maxSetsPerPool},
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer, maxSetsPerPool},
        vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage, maxSetsPerPool},
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage, maxSetsPerPool}
    };
    vktg::DescriptorSetAllocator descriptorsetAllocator( poolSizes, maxSetsPerPool);
    vktg::DescriptorLayoutCache descriptorSetLayoutCache;

    deletionStack.Push( [&](){
        descriptorSetLayoutCache.DestroyLayouts();
        descriptorsetAllocator.DestroyPools();
    });


    // pipelines
    vk::ShaderModule computeShader = vktg::LoadShader( "res/shaders/gradient_comp.spv");
    auto edgeColors = vk::PushConstantRange{}
        .setStageFlags( vk::ShaderStageFlagBits::eCompute )
        .setOffset( 0 )
        .setSize( 4 * 4 * sizeof(float) );
    vk::DescriptorSetLayout computeLayout;
    vk::DescriptorImageInfo computeImageInfo = vktg::GetDescriptorImageInfo( renderImage.imageView, VK_NULL_HANDLE, vk::ImageLayout::eGeneral);
    vk::DescriptorSet computeSet = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
        .BindImage( 0, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, &computeImageInfo)
        .Build( &computeLayout);
    vktg::Pipeline computePipeline = vktg::ComputePipelineBuilder()
        .SetShader( computeShader )
        .AddDescriptorLayout( computeLayout )
        .AddPushConstant( edgeColors )
        .Build();

    deletionStack.Push( [=](){
        vktg::DestroyPipelineLayout( computePipeline.pipelineLayout);
        vktg::DestroyPipeline( computePipeline.pipeline);
    });

    vk::ShaderModule vertexShader = vktg::LoadShader( "res/shaders/triangle_vert.spv");
    vk::ShaderModule fragmentShader = vktg::LoadShader( "res/shaders/triangle_frag.spv");
    vktg::Pipeline trianglePipeline = vktg::GraphicsPipelineBuilder()
        .AddShader( vertexShader, vk::ShaderStageFlagBits::eVertex)
        .AddShader( fragmentShader, vk::ShaderStageFlagBits::eFragment)
        .Build();


    // synchronization objects
    vk::Fence renderFence = vktg::CreateFence();
    vk::Semaphore renderSemaphore = vktg::CreateSemaphore();
    vk::Semaphore presentSemaphore = vktg::CreateSemaphore();

    deletionStack.Push( [=](){
        vktg::DestroyFence( renderFence);
        vktg::DestroySemaphore( renderSemaphore);
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
    vktg::DestroyImage( renderImage);
    vktg::DestroySwapchain( swapchain);

    vktg::ShutDown();


    return 0;
}