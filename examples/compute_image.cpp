
#include "vulkantogo.h"

#include <cmath>


int main() {

    vktg::StartUp();


    // deletion stack
    auto deletionStack = vktg::DeletionStack{};


     // swapchain
    auto swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);


    // render image
    vktg::Image renderImage;
    vktg::CreateImage(
        renderImage,
        swapchain.Width(), swapchain.Height(), vk::Format::eR16G16B16A16Sfloat, 
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
    );


    // descriptors
    uint32_t maxSetsPerPool = 10;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage, maxSetsPerPool}
    };
    vktg::DescriptorSetAllocator descriptorsetAllocator( poolSizes, maxSetsPerPool);
    vktg::DescriptorLayoutCache descriptorSetLayoutCache;

    deletionStack.Push( [&](){
        descriptorSetLayoutCache.DestroyLayouts();
        descriptorsetAllocator.DestroyPools();
    });


    // compute pipeline
    // load shader
    auto computeShader = vktg::LoadShader( "../res/shaders/gradient_comp.spv");
    // push constants
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
    // descriptor set
    vk::DescriptorSetLayout computeLayout;
    auto computeImageInfo = vktg::GetDescriptorImageInfo( renderImage.imageView, VK_NULL_HANDLE, vk::ImageLayout::eGeneral);
    auto computeDescriptors = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
        .BindImage( 0, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, &computeImageInfo )
        .Build( &computeLayout );
    // build pipeline
    auto computePipeline = vktg::ComputePipelineBuilder()
        .SetShader( computeShader )
        .AddDescriptorLayout( computeLayout )
        .AddPushConstant( computePush )
        .Build();

    // cleanup shader modules immediately, no longer needed
    vktg::DestroyShaderModule( computeShader);

    deletionStack.Push( [=](){
        vktg::DestroyPipelineLayout( computePipeline.pipelineLayout);
        vktg::DestroyPipeline( computePipeline.pipeline);
    });


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
        	vktg::WaitIdle();

            vktg::CreateSwapchain( swapchain);
            vktg::ResizeImage( renderImage, swapchain.Width(), swapchain.Height());

            // update descriptors
            descriptorsetAllocator.ResetPools();
            computeImageInfo = vktg::GetDescriptorImageInfo( renderImage.imageView, VK_NULL_HANDLE, vk::ImageLayout::eGeneral);
            computeDescriptors = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
                .BindImage( 0, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, &computeImageInfo)
                .Build();
        }

        // get current frame
        auto &frame = frameResources[frameCount % frameOverlap];
            
        // wait for render fence to record render commands
        vktg::WaitForFence( frame.renderFence);
        vktg::ResetFence( frame.renderFence);
            
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

            // update background colors
            float tlr = std::cos( frameCount / 1013.f), tlg = std::sin( frameCount / 1013.f);
            float trg = std::cos( frameCount / 907.f), trb = std::sin( frameCount / 907.f);
            float blb = std::cos( frameCount / 1129.f), blr = std::sin( frameCount / 1129.f);
            float brw = std::cos( frameCount / 491.f);
            cornerColors.tl[0] = tlr*tlr; cornerColors.tl[1] = tlg*tlg;
            cornerColors.tr[1] = trg*trg; cornerColors.tl[2] = trb*trb;
            cornerColors.bl[2] = blb*blb; cornerColors.bl[0] = blr*blr;
            cornerColors.br[0] = cornerColors.br[1] = cornerColors.br[2] = brw*brw;;

            //background compute draw
            vktg::TransitionImageLayout( 
                cmd, renderImage.image, 
                vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
                vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
                vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderStorageWrite
            );

            cmd.bindPipeline( vk::PipelineBindPoint::eCompute, computePipeline.pipeline);
            cmd.bindDescriptorSets( vk::PipelineBindPoint::eCompute, computePipeline.pipelineLayout, 0, 1, &computeDescriptors, 0, nullptr);
            cmd.pushConstants( computePipeline.pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(ShaderPushConstants), &cornerColors);
            cmd.dispatch( std::ceil(renderImage.Width() / 16.f), std::ceil(renderImage.Height() / 16.f), 1);

            // copy render image to swapchain
            vktg::TransitionImageLayout( 
                cmd, renderImage.image, 
                vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
                vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderWrite,
                vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferRead
            );
            vktg::TransitionImageLayout( 
                cmd, swapchain.images[imageIndex], 
                vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
                vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite
            );
            vktg::CopyImage( 
                cmd, renderImage.image, swapchain.images[imageIndex],
                vk::Rect2D{vk::Offset2D{0, 0}, vk::Extent2D{renderImage.Width(), renderImage.Height()}},
                vk::Rect2D{vk::Offset2D{0, 0}, vk::Extent2D{swapchain.Width(), swapchain.Height()}}
            );

            // transition swapchain image to present optimal layout
            vktg::TransitionImageLayout( 
                cmd, swapchain.images[imageIndex], 
                vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
                vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone
            );

        cmd.end();

        // submit
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

        // present
        vktg::PresentImage( swapchain, &frame.presentSemaphore, &imageIndex);

        ++frameCount;
    }


    // cleanup
	vktg::WaitIdle();

    deletionStack.Flush();
    vktg::DestroyImage( renderImage);
    vktg::DestroySwapchain( swapchain);

    vktg::ShutDown();


    return 0;
}