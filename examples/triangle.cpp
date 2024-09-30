
#include "vulkantogo.h"

#include <cmath>
#include <vector>


int main() {

    vktg::StartUp();


    // deletion stack
    vktg::DeletionStack deletionStack = vktg::DeletionStack{};


    // swapchain
    auto swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    // render image
    vktg::Image renderImage;
    vktg::CreateImage(
        renderImage,
        swapchain.extent.width, swapchain.extent.height, vk::Format::eR16G16B16A16Sfloat, 
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
    );


    // graphics pipeline
    auto vertexShader = vktg::LoadShader( "../res/shaders/triangle_vert.spv");
    auto fragmentShader = vktg::LoadShader( "../res/shaders/triangle_frag.spv");
    std::vector<vk::Format> colorattachmentFormats = {renderImage.imageInfo.format};
    auto trianglePipeline = vktg::GraphicsPipelineBuilder()
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

            // geometry draw
            vktg::TransitionImageLayout( 
                cmd, renderImage.image, 
                vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
                vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderStorageWrite,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite
            );

            auto clearValue = vk::ClearValue{}.setColor( vk::ClearColorValue().setFloat32( {0.f, 0.f, 0.f, 1.f}) );
            std::vector<vk::RenderingAttachmentInfo> colorAttachments = {vktg::CreateRenderingAttachment( renderImage.imageView, &clearValue)};
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
                cmd, renderImage.image, 
                vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite,
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