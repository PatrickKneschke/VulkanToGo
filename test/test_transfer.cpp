
#include <catch2/catch.hpp>

#include "../vulkantogo/transfer.h"
#include "../vulkantogo/storage.h"
#include "../vulkantogo/commands.h"
#include "../vulkantogo/synchronization.h"


TEST_CASE( "copy buffer to buffer", "[transfer]") {

    vktg::Buffer buffer1;
    vktg::CreateBuffer( buffer1, 16, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);
    vktg::Buffer buffer2;
    vktg::CreateBuffer( buffer2, 16, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);

    float data[4] = {1.f, 2.f, 3.f, 4.f};
    float *ptr1 = reinterpret_cast<float*>( buffer1.Data());
    for (int i=0; i<4; i++)
    {
        ptr1[i] = data[i];
    }

    vk::CommandPool cmdPool = vktg::CreateCommandPool( vktg::TransferQueueIndex());
    vk::CommandBuffer cmd = vktg::AllocateCommandBuffer( cmdPool);
    vk::Fence fence = vktg::CreateFence( vk::FenceCreateFlags{});

    cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);
    auto cmdBeginInfo = vk::CommandBufferBeginInfo{}
        .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );		
    cmd.begin( cmdBeginInfo);

        vktg::CopyBuffer( cmd, buffer1.buffer, buffer2.buffer, 16);

    cmd.end();


    vk::CommandBufferSubmitInfo cmdInfos[] = {
        vk::CommandBufferSubmitInfo{}
            .setCommandBuffer( cmd )
    };
    vktg::SubmitCommands( vktg::TransferQueue(), cmdInfos, {}, {}, fence);
    vktg::WaitForFence( fence);
    

    float *ptr2 = reinterpret_cast<float*>( buffer2.Data());
    for (int i=0; i<4; i++)
    {
        REQUIRE( ptr2[i] == data[i] );
    }


    vktg::DestroyCommandPool( cmdPool);
    vktg::DestroyFence( fence);
}


TEST_CASE( "copy buffer regions to buffer", "[transfer]") {

    vktg::Buffer buffer1;
    vktg::CreateBuffer( buffer1, 16, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);
    vktg::Buffer buffer2;
    vktg::CreateBuffer( buffer2, 16, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);

    float *ptr1 = reinterpret_cast<float*>( buffer1.Data());
    for (int i=0; i<4; i++)
    {
        ptr1[i] = (float)i;
    }

    float *ptr2 = reinterpret_cast<float*>( buffer2.Data());
    for (int i=0; i<4; i++)
    {
        ptr2[i] = 4.f;
    }


    vk::CommandPool cmdPool = vktg::CreateCommandPool( vktg::TransferQueueIndex());
    vk::CommandBuffer cmd = vktg::AllocateCommandBuffer( cmdPool);
    vk::Fence fence = vktg::CreateFence( vk::FenceCreateFlags{});

    cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);
    auto cmdBeginInfo = vk::CommandBufferBeginInfo{}
        .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );		
    cmd.begin( cmdBeginInfo);

        vk::BufferCopy2 regions[2] = {
            vk::BufferCopy2{}
                .setSize( 4 )
                .setSrcOffset( 4 )
                .setDstOffset( 0 ),
            vk::BufferCopy2{}
                .setSize( 4 )
                .setSrcOffset( 12 )
                .setDstOffset( 8 )
        };
        vktg::CopyBuffer( cmd, buffer1.buffer, buffer2.buffer, regions);

    cmd.end();

    
    vk::CommandBufferSubmitInfo cmdInfos[] = {
        vk::CommandBufferSubmitInfo{}
            .setCommandBuffer( cmd )
    };
    vktg::SubmitCommands( vktg::TransferQueue(), cmdInfos, {}, {}, fence);
    vktg::WaitForFence( fence);


    REQUIRE( ptr2[0] == 1.f );
    REQUIRE( ptr2[1] == 4.f );
    REQUIRE( ptr2[2] == 3.f );
    REQUIRE( ptr2[3] == 4.f );


    vktg::DestroyCommandPool( cmdPool);
    vktg::DestroyFence( fence);
}
