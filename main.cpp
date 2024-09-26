
#include "vulkantogo.h"

#include <cmath>
#include <iostream>
#include <vector>


int main() {

    vktg::StartUp();


    // deletion stack
    vktg::DeletionStack deletionStack = vktg::DeletionStack{};


    // swapchain
    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    vktg::Image renderImage;
    vktg::CreateImage(
        renderImage,
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


    // compute pipeline
    vk::ShaderModule computeShader = vktg::LoadShader( "../res/shaders/gradient_comp.spv");

    struct ShaderPushConstants {
        float tl[4];
        float tr[4];
        float bl[4];
        float br[4];
    } cornerColors {
        .tl = {1.f, 0.f, 0.f, 1.f},
        .tr = {0.f, 1.f, 0.f, 1.f},
        .bl = {0.f, 0.f, 1.f, 1.f},
        .br = {0.5f, 0.5f, 0.5f, 1.f}
    };

    auto computePush = vk::PushConstantRange{}
        .setStageFlags( vk::ShaderStageFlagBits::eCompute )
        .setOffset( 0 )
        .setSize( sizeof(ShaderPushConstants) );

    vk::DescriptorSetLayout computeLayout;
    vk::DescriptorImageInfo computeImageInfo = vktg::GetDescriptorImageInfo( renderImage.imageView, VK_NULL_HANDLE, vk::ImageLayout::eGeneral);
    vk::DescriptorSet computeSet = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
        .BindImage( 0, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, &computeImageInfo )
        .Build( &computeLayout );

    vktg::Pipeline computePipeline = vktg::ComputePipelineBuilder()
        .SetShader( computeShader )
        .AddDescriptorLayout( computeLayout )
        .AddPushConstant( computePush )
        .Build();

    deletionStack.Push( [=](){
        vktg::DestroyPipelineLayout( computePipeline.pipelineLayout);
        vktg::DestroyPipeline( computePipeline.pipeline);
    });

    // graphics pipeline
    vk::ShaderModule vertexShader = vktg::LoadShader( "../res/shaders/triangle_vert.spv");
    vk::ShaderModule fragmentShader = vktg::LoadShader( "../res/shaders/triangle_frag.spv");
    std::vector<vk::Format> colorattachmentFormats = {renderImage.imageInfo.format};
    vktg::Pipeline trianglePipeline = vktg::GraphicsPipelineBuilder()
        .AddShader( vertexShader, vk::ShaderStageFlagBits::eVertex )
        .AddShader( fragmentShader, vk::ShaderStageFlagBits::eFragment )
        .SetDynamicStates( {vk::DynamicState::eViewport, vk::DynamicState::eScissor} )
        .SetInputAssembly( vk::PrimitiveTopology::eTriangleList )
        .SetPolygonMode( vk::PolygonMode::eFill )
        .SetCulling( vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise )
        .SetColorFormats( colorattachmentFormats )
        .Build();

    deletionStack.Push( [=](){
        vktg::DestroyPipelineLayout( trianglePipeline.pipelineLayout);
        vktg::DestroyPipeline( trianglePipeline.pipeline);
    });

    // cleanup shader modules immediately, no longer needed
    vktg::DestroyShaderModule( computeShader);
    vktg::DestroyShaderModule( vertexShader);
    vktg::DestroyShaderModule( fragmentShader);


    // frame resources
    uint64_t frameCount = 0;
    const uint8_t frameOverlap = 2;
    struct FrameResources {
        vk::Fence renderFence;
        vk::Semaphore renderSemaphore;
        vk::Semaphore presentSemaphore;
        vk::CommandPool commandPool;
        vk::CommandBuffer commandbuffer;
    } frameResources[frameOverlap];

    for (auto &frame : frameResources)
    {
        // synchronization objects
        frame.renderFence = vktg::CreateFence();
        frame.renderSemaphore = vktg::CreateSemaphore();
        frame.presentSemaphore = vktg::CreateSemaphore();
        deletionStack.Push( [=](){
            vktg::DestroyFence( frame.renderFence);
            vktg::DestroySemaphore( frame.renderSemaphore);
            vktg::DestroySemaphore( frame.presentSemaphore);
        });
        
        // command pools and command buffers
        frame.commandPool = vktg::CreateCommandPool( vktg::GraphicsQueueIndex());
        deletionStack.Push( [=](){
            vktg::DestroyCommandPool( frame.commandPool);
        });
        frame.commandbuffer = vktg::AllocateCommandBuffer( frame.commandPool);
    }


    // render loop
    while (!glfwWindowShouldClose( vktg::Window())) 
    {
        glfwPollEvents();

        // recreate swapchain and render image if outdated
        if (!swapchain.isValid)
        {
        	vktg::Device().waitIdle();

            vktg::CreateSwapchain( swapchain);
            vktg::DestroyImage( renderImage);
            vktg::CreateImage(
                renderImage,
                swapchain.extent.width, swapchain.extent.height, vk::Format::eR16G16B16A16Sfloat, 
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
            );

            // update descriptors
            descriptorsetAllocator.ResetPools();
            computeImageInfo = vktg::GetDescriptorImageInfo( renderImage.imageView, VK_NULL_HANDLE, vk::ImageLayout::eGeneral);
            computeSet = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
                .BindImage( 0, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, &computeImageInfo)
                .Build();
        }

        // get current frame
        auto &frame = frameResources[frameCount % frameOverlap];
            
        // wait for render fence to record render commands
        VK_CHECK( vktg::Device().waitForFences( frame.renderFence, true, 1e9) );
        VK_CHECK( vktg::Device().resetFences( 1, &frame.renderFence) );
            
        // get next swapchain image
        uint32_t imageIndex;
        if (!vktg::NextSwapchainImage( swapchain, frame.renderSemaphore, &imageIndex))
        {
            continue;
        }

        // record render commands
        auto cmd = frame.commandbuffer;
        cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);
        auto commandBeginInfo = vk::CommandBufferBeginInfo{}
            .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
        VK_CHECK( cmd.begin( &commandBeginInfo) );

            //background compute draw
            vktg::TransitionImageLayout( 
                cmd, renderImage.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
                vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
                vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderStorageWrite
            );

            cmd.bindPipeline( vk::PipelineBindPoint::eCompute, computePipeline.pipeline);
            cmd.bindDescriptorSets( vk::PipelineBindPoint::eCompute, computePipeline.pipelineLayout, 0, 1, &computeSet, 0, nullptr);
            cmd.pushConstants( computePipeline.pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(ShaderPushConstants), &cornerColors);
            cmd.dispatch( std::ceil(renderImage.Width() / 16), std::ceil(renderImage.Height() / 16), 1);

            // geometry draw
            vktg::TransitionImageLayout( 
                cmd, renderImage.image, vk::ImageLayout::eGeneral, vk::ImageLayout::eColorAttachmentOptimal,
                vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderStorageWrite,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite
            );

            std::vector<vk::RenderingAttachmentInfo> colorAttachments = {vktg::CreateRenderingAttachment( renderImage.imageView)};
            auto renderingInfo = vktg::CreateRenderingInfo( swapchain.extent, colorAttachments);
            cmd.beginRendering( renderingInfo);

                cmd.bindPipeline( vk::PipelineBindPoint::eGraphics, trianglePipeline.pipeline);
                
                //set dynamic viewport and scissor
                vk::Viewport viewport = vktg::CreateViewport( 0.f, 0.f, renderImage.Width(), renderImage.Height(), 0.f, 1.f);
                vk::Rect2D scissor = vktg::CreateScissor( 0.f, 0.f, renderImage.Width(), renderImage.Height());
                cmd.setViewport( 0, 1, &viewport);
                cmd.setScissor( 0, 1, &scissor);

                cmd.draw( 3, 1, 0, 0);

            cmd.endRendering();


            // copy render image to swapchain
            vktg::TransitionImageLayout( 
                cmd, renderImage.image, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite,
                vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferRead
            );
            vktg::TransitionImageLayout( 
                cmd, swapchain.images[imageIndex], vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
                vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite
            );
            vktg::CopyImage( 
                cmd, renderImage.image, swapchain.images[imageIndex],
                vk::Rect2D{vk::Offset2D{0, 0}, vk::Extent2D{renderImage.Width(), renderImage.Height()}},
                vk::Rect2D{vk::Offset2D{0, 0}, vk::Extent2D{swapchain.extent.width, swapchain.extent.height}}
            );

            // transition swapchain image to present optimal layout
            vktg::TransitionImageLayout( 
                cmd, swapchain.images[imageIndex], vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
                vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone
            );

        cmd.end();


        vk::CommandBufferSubmitInfo cmdInfos[] = {
            vk::CommandBufferSubmitInfo{}
                .setCommandBuffer( cmd )
        };
        vk::SemaphoreSubmitInfo waitInfos[] = {
            vk::SemaphoreSubmitInfo{}
                .setSemaphore( frame.renderSemaphore )
                .setStageMask( vk::PipelineStageFlagBits2::eColorAttachmentOutput )
        };
        vk::SemaphoreSubmitInfo signalInfos[] = {
            vk::SemaphoreSubmitInfo{}
                .setSemaphore( frame.presentSemaphore )
        };
        vktg::SubmitCommands( vktg::GraphicsQueue(), cmdInfos, waitInfos, signalInfos, frame.renderFence);

        vktg::PresentImage( swapchain, &frame.presentSemaphore, &imageIndex);

        ++frameCount;
    }


    // cleanup
	vktg::Device().waitIdle();

    deletionStack.Flush();
    vktg::DestroyImage( renderImage);
    vktg::DestroySwapchain( swapchain);

    vktg::ShutDown();


    return 0;
}