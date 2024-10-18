

#include "vulkantogo.h"
#include "shared/utility.h"

#include <glm/gtx/euler_angles.hpp>

#include <cmath>
#include <vector>

#include <iostream>


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
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
    );
    // depth image
    vktg::Image depthImage;
    vktg::CreateImage( 
        depthImage, 
        swapchain.extent.width, swapchain.extent.height, vk::Format::eD24UnormS8Uint,
        vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth
    );

    // clearn values
    auto clearColor = vktg::CreateClearColorValue( 0.2f, 0.2f, 0.2f, 1.0f);
    auto clearDepth = vktg::CreateClearDepthStencilValue( 1.f, 0.f);


    // descriptors
    uint32_t maxSetsPerPool = 10;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, maxSetsPerPool},
        vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage, maxSetsPerPool}
    };
    vktg::DescriptorSetAllocator descriptorsetAllocator( poolSizes, maxSetsPerPool);
    vktg::DescriptorLayoutCache descriptorSetLayoutCache;

    deletionStack.Push( [&](){
        descriptorSetLayoutCache.DestroyLayouts();
        descriptorsetAllocator.DestroyPools();
    });


    // uniform buffers
    // camera
    struct Camera {
        glm::vec3 pos;
        glm::vec3 forward;
        glm::vec3 up;
        float fov;
        float aspect;
        float zNear;
        float zFar;
    } camera{
        .pos = {2.f, 2.f, 2.f},
        .forward = {-1.f, -1.f, -1.f},
        .up = {0.f, 0.f, 1.f},
        .fov = glm::radians( 60.f),
        .aspect = (float)renderImage.Width() / renderImage.Height(),
        .zNear = 0.1f,
        .zFar = 100.f
    };
    struct CameraData {
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 viewProj;
        glm::vec3 pos;
    } cameraData;
    vktg::Buffer cameraBuffer;
    vktg::CreateBuffer( cameraBuffer, sizeof( CameraData), vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);

    deletionStack.Push( [&](){
        vktg::DestroyBuffer( cameraBuffer);
    });

    // render object
    struct Object {
        glm::vec3 pos;
        glm::vec3 rotation;
        glm::vec3 scale;
    } object {
        .pos = {0.f, 0.f, 0.f},
        .rotation = {0.f, 0.f, 0.f},
        .scale = {1.f, 1.f, 1.f}
    };
    struct ObjectData {
        glm::mat4 model = glm::mat4(1.f);
    } objectData;
    vktg::Buffer objectBuffer;
    vktg::CreateBuffer( objectBuffer, sizeof( ObjectData), vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);

    deletionStack.Push( [&](){
        vktg::DestroyBuffer( objectBuffer);
    });

    // model
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    LoadMesh( "../res/models/viking_room.obj", vertices, indices);
    vktg::Buffer vertexBuffer, indexBuffer;
    vktg::CreateBuffer( vertexBuffer, vertices.size() * sizeof(Vertex), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);
    vktg::CreateBuffer( indexBuffer, indices.size() * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);
    
    deletionStack.Push( [&](){
        vktg::DestroyBuffer( vertexBuffer);
        vktg::DestroyBuffer( indexBuffer);
    });

    // texture
    uint32_t width, height;
    auto imgData = LoadImage( "../res/images/viking_room.png", width, height);
    vktg::Image texture;
    vktg::CreateImage( texture, width, height, vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);

    deletionStack.Push( [&](){
        vktg::DestroyImage( texture);
    });

    // sampler
    auto sampler = vktg::SamplerBuilder()
        .SetFilter( vk::Filter::eLinear )
        .SetAddressMode( vk::SamplerAddressMode::eClampToBorder )
        .SetBorderColor( vk::BorderColor::eIntOpaqueBlack )
        .Build();
    
    deletionStack.Push( [=](){
        vktg::DestroySampler( sampler);
    });


    // graphics pipeline
    // shaders
    auto vertexShader = vktg::LoadShader( "../res/shaders/mesh_vert.spv");
    auto fragmentShader = vktg::LoadShader( "../res/shaders/texture_frag.spv");
    // descriptor sets
    vk::DescriptorSetLayout cameraDescriptorLayout;
    auto cameraBufferInfo = vktg::GetDescriptorBufferInfo( cameraBuffer.buffer);
    auto cameraDescriptor = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
        .BindBuffer( 0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, &cameraBufferInfo )
        .Build( &cameraDescriptorLayout );
    vk::DescriptorSetLayout objectDescriptorLayout;
    auto objectBufferInfo = vktg::GetDescriptorBufferInfo( objectBuffer.buffer);
    auto objectDescriptor = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
        .BindBuffer( 0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, &objectBufferInfo )
        .Build( &objectDescriptorLayout );
    vk::DescriptorSetLayout textureDescriptorLayout;
    auto textureInfo = vktg::GetDescriptorImageInfo( texture.imageView, sampler);
    auto textureDescriptor = vktg::DescriptorSetBuilder( &descriptorsetAllocator, &descriptorSetLayoutCache)
        .BindImage( 0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, &textureInfo )
        .Build( &textureDescriptorLayout );
    // build pipeline
    auto vertexAttributes = Vertex::getAttributeDescriptions();
    std::vector<vk::Format> colorattachmentFormats = {renderImage.Format()};
    std::vector<vk::DescriptorSetLayout> descriptorLayouts = {cameraDescriptorLayout, objectDescriptorLayout, textureDescriptorLayout};
    auto texturedMeshPipeline = vktg::GraphicsPipelineBuilder()
        .AddShader( vertexShader, vk::ShaderStageFlagBits::eVertex )
        .AddShader( fragmentShader, vk::ShaderStageFlagBits::eFragment )
        .AddDescriptorLayouts( descriptorLayouts )
        .SetDynamicStates( {vk::DynamicState::eViewport, vk::DynamicState::eScissor} )
        .SetVertexInputBindng( Vertex::getBindingDescription() )
        .SetVertexAttributes( vertexAttributes )
        .SetInputAssembly( vk::PrimitiveTopology::eTriangleList )
        .SetPolygonMode( vk::PolygonMode::eFill )
        .SetCulling( vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise )
        .EnableDepth()
        .SetDepthFormat( depthImage.Format() )
        .SetColorFormats( colorattachmentFormats )
        .Build();


    deletionStack.Push( [=](){
        vktg::DestroyPipelineLayout( texturedMeshPipeline.pipelineLayout);
        vktg::DestroyPipeline( texturedMeshPipeline.pipeline);
    });

    // cleanup shader modules immediately, no longer needed
    vktg::DestroyShaderModule( vertexShader);
    vktg::DestroyShaderModule( fragmentShader);

    // upload data to gpu
    vktg::Buffer vertexStaging;
    vktg::CreateStagingBuffer( vertexStaging, vertices.size() * sizeof(Vertex), vertices.data());
    vktg::Buffer indexStaging;
    vktg::CreateStagingBuffer( indexStaging, indices.size() * sizeof(uint32_t), indices.data());
    vktg::Buffer cameraStaging;
    vktg::CreateStagingBuffer( cameraStaging, sizeof(CameraData), &cameraData);
    vktg::Buffer objectStaging;
    vktg::CreateStagingBuffer( objectStaging, sizeof(ObjectData), &objectData);
    vktg::Buffer textureStaging;
    vktg::CreateStagingBuffer( textureStaging, imgData.size() * sizeof(uint8_t), imgData.data());

    deletionStack.Push( [=](){
        vktg::DestroyBuffer( cameraStaging);
        vktg::DestroyBuffer( objectStaging);
    });

    vktg::SubmitContext transferSubmit = vktg::CreateSubmitContext( vktg::QueueType::eTransfer);
    deletionStack.Push( [&](){
        vktg::DestroySubmitContext( transferSubmit);
    });

    transferSubmit.Begin();
        vktg::CopyBuffer( transferSubmit.cmd, vertexStaging.buffer, vertexBuffer.buffer, vertexStaging.Size());
        vktg::CopyBuffer( transferSubmit.cmd, indexStaging.buffer, indexBuffer.buffer, indexStaging.Size());
        vktg::CopyBuffer( transferSubmit.cmd, cameraStaging.buffer, cameraBuffer.buffer, cameraStaging.Size());
        vktg::CopyBuffer( transferSubmit.cmd, objectStaging.buffer, objectBuffer.buffer, objectStaging.Size());
        vktg::TransitionImageLayout( 
            transferSubmit.cmd, texture.image, 
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
            vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite
        );
        vktg::CopyBufferToImage( transferSubmit.cmd, textureStaging.buffer, texture.image, 0, texture.Width(), texture.Height());
        vktg::TransitionImageLayout( 
            transferSubmit.cmd, texture.image, 
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite,
            vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone
        );
    transferSubmit.End();
    transferSubmit.Submit();

    // wait for data transfer to complete
    VK_CHECK( vktg::Device().waitForFences( 1, &transferSubmit.fence, true, 1e9) );

    // cleanup staging buffers that are no longer needed
    vktg::DestroyBuffer( vertexStaging);
    vktg::DestroyBuffer( indexStaging);
    vktg::DestroyBuffer( textureStaging);


    // frame handler
    const uint8_t frameOverlap = 2;
    vktg::FrameHandler frameHandler( frameOverlap);
    frameHandler.RegisterLateCallback( "fps", [&](){

        static double totalTime = 0.0;
        static uint64_t lastFrameCount = 0;
        
        totalTime += frameHandler.DeltaTime();

        if (totalTime >= 1.0)
        {
            std::cout << "fps = " << frameHandler.FrameCount() - lastFrameCount << '\n';
            lastFrameCount = frameHandler.FrameCount();
            totalTime = 0.0;
        }
    });

    // frame resources
    struct FrameResources {
        vk::Fence renderFence;
        vk::Semaphore renderSemaphore;
        vk::Semaphore presentSemaphore;
        vk::CommandPool commandPool;
        vk::CommandBuffer commandbuffer;
    } frameResources[frameHandler.FrameOverlap()];

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


    // input handler
    auto input = vktg::InputHandler( vktg::Window());
    input.CaptureCursor();

    auto inputLayer = vktg::InputLayer();
    inputLayer.keyCallback = [&](const bool *keyPress) {

        if (keyPress[GLFW_KEY_ESCAPE])
        {
            glfwSetWindowShouldClose( vktg::Window(), true);
        }

        if (keyPress[GLFW_KEY_LEFT])
        {
            object.rotation[2] -= (keyPress[GLFW_KEY_LEFT_CONTROL]  ?  60  :  20) * frameHandler.DeltaTime();
        }
        
        if (keyPress[GLFW_KEY_RIGHT])
        {
            object.rotation[2] += (keyPress[GLFW_KEY_LEFT_CONTROL]  ?  60  :  20) * frameHandler.DeltaTime();
        }
    };
    inputLayer.cursorPosCallback = [&](double x, double y, double dx, double dy){

        static float sensitivity = 0.2f;
        auto right = glm::cross( -camera.forward, camera.up);
        float yaw = glm::radians( -(float)dx) * sensitivity;
        float pitch = glm::radians( (float)dy) * sensitivity;
        camera.forward = glm::mat3( glm::rotate( glm::mat4(1.f), yaw, camera.up) ) * camera.forward;
        camera.forward = glm::mat3( glm::rotate( glm::mat4(1.f), pitch, right) ) * camera.forward;
    };
    input.PushLayer( &inputLayer); 


    // render loop
    while (!glfwWindowShouldClose( vktg::Window())) 
    {
        // early frame updates
        frameHandler.EarlyUpdate();


        // handle input
        input.Update();


        // recreate swapchain and render image if outdated
        if (!swapchain.isValid)
        {
        	vktg::Device().waitIdle();

            vktg::CreateSwapchain( swapchain);
            vktg::DestroyImage( renderImage);
            vktg::CreateImage(
                renderImage,
                swapchain.extent.width, swapchain.extent.height, vk::Format::eR16G16B16A16Sfloat, 
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
            );
            vktg::DestroyImage( depthImage);
            vktg::CreateImage(
                depthImage,
                swapchain.extent.width, swapchain.extent.height, vk::Format::eD24UnormS8Uint, 
                vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth
            );
        }


        // update uniform buffers
        objectData.model = glm::eulerAngleXYZ( glm::radians(object.rotation[0]), glm::radians(object.rotation[1]), glm::radians(object.rotation[2]));
        objectData.model = glm::scale( objectData.model, object.scale);
        objectData.model = glm::translate( objectData.model, object.pos);
        memcpy( objectStaging.allocationInfo.pMappedData, &objectData, sizeof(ObjectData));

        cameraData.pos = camera.pos;
        cameraData.view = glm::lookAt( camera.pos, camera.pos + camera.forward, camera.up);
        cameraData.proj = glm::perspective( camera.fov, camera.aspect, camera.zNear, camera.zFar);
        cameraData.proj[1][1] *= -1;
        cameraData.viewProj = cameraData.proj * cameraData.view;
        memcpy( cameraStaging.allocationInfo.pMappedData, &cameraData, sizeof(CameraData));

        transferSubmit.Begin();
            vktg::CopyBuffer( transferSubmit.cmd, objectStaging.buffer, objectBuffer.buffer, objectStaging.Size());
            vktg::CopyBuffer( transferSubmit.cmd, cameraStaging.buffer, cameraBuffer.buffer, cameraStaging.Size());
        transferSubmit.End();
        transferSubmit.Submit();


        // get current frame
        auto &frame = frameResources[frameHandler.CurrentFrameIndex()];
            
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

            std::vector<vk::RenderingAttachmentInfo> colorAttachments = {vktg::CreateColorAttachment( renderImage.imageView, &clearColor)};
            vk::RenderingAttachmentInfo depthAttachment = vktg::CreateDepthStencilAttachment( depthImage.imageView, &clearDepth);           
            auto renderingInfo = vktg::CreateRenderingInfo( swapchain.extent, colorAttachments, &depthAttachment);
            cmd.beginRendering( renderingInfo);

                size_t offset = 0;
                cmd.bindVertexBuffers2( 0, 1, &vertexBuffer.buffer, &offset, nullptr, nullptr);
                cmd.bindIndexBuffer( indexBuffer.buffer, 0, vk::IndexType::eUint32);

                cmd.bindPipeline( vk::PipelineBindPoint::eGraphics, texturedMeshPipeline.pipeline);
                vk::DescriptorSet descriptors[] = {cameraDescriptor, objectDescriptor, textureDescriptor};
                cmd.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, texturedMeshPipeline.pipelineLayout, 0, 3, descriptors, 0, nullptr);
                
                //set dynamic viewport and scissor
                vk::Viewport viewport = vktg::CreateViewport( 0.f, 0.f, renderImage.Width(), renderImage.Height(), 0.f, 1.f);
                vk::Rect2D scissor = vktg::CreateScissor( 0.f, 0.f, renderImage.Width(), renderImage.Height());
                cmd.setViewport( 0, 1, &viewport);
                cmd.setScissor( 0, 1, &scissor);

                cmd.drawIndexed( (uint32_t)indices.size(), 1, 0, 0, 0);


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


        // wait for data transfer to complete
        VK_CHECK( vktg::Device().waitForFences( 1, &transferSubmit.fence, true, 1e9) );


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

        
        // late frame updates
        frameHandler.LateUpdate();
    }


    // cleanup
	vktg::Device().waitIdle();

    deletionStack.Flush();
    vktg::DestroyImage( renderImage);
    vktg::DestroyImage( depthImage);
    vktg::DestroySwapchain( swapchain);

    vktg::ShutDown();


    return 0;
}
