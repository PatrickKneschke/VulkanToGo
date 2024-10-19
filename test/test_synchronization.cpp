
#include <catch2/catch.hpp>

#include "../vulkantogo/synchronization.h"
#include "../vulkantogo/storage.h"


TEST_CASE("create fence", "[synchronization]") {

    vk::Fence fence = vktg::CreateFence();
    vk::Result result = vktg::Device().getFenceStatus( fence);

    REQUIRE_FALSE( !fence);
    REQUIRE( result == vk::Result::eSuccess);

    vktg::DestroyFence( fence);
}


TEST_CASE("create semaphore", "[synchronization]") {

    vk::Semaphore semaphore = vktg::CreateSemaphore( vk::SemaphoreType::eTimeline);

    uint64_t value = 123;
    vktg::SignalSemaphore( semaphore, value);
    
    uint64_t counter;
    VK_CHECK( vktg::Device().getSemaphoreCounterValue( semaphore, &counter) );

    REQUIRE_FALSE( !semaphore);
    REQUIRE( counter == value);

    vktg::DestroySemaphore( semaphore);
}


TEST_CASE("create memory barrier", "[synchronization]") {

    vk::MemoryBarrier2 barrier = vktg::CreateMemoryBarrier( 
        vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite,
        vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderStorageRead
    );

    REQUIRE( barrier.srcStageMask == vk::PipelineStageFlagBits2::eTransfer );
    REQUIRE( barrier.srcAccessMask == vk::AccessFlags2{ vk::AccessFlagBits2::eTransferWrite} );
    REQUIRE( barrier.dstStageMask == vk::PipelineStageFlagBits2::eComputeShader );
    REQUIRE( barrier.dstAccessMask == vk::AccessFlags2{ vk::AccessFlagBits2::eShaderStorageRead} );
}


TEST_CASE("create buffer memory barrier", "[synchronization]") {

    vktg::Buffer buffer;
    vktg::CreateBuffer( buffer, 256, vk::BufferUsageFlagBits::eUniformBuffer);

    vk::BufferMemoryBarrier2 barrier = vktg::CreateBufferMemoryBarrier(
        buffer.buffer, 
        vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite,
        vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eShaderStorageRead,
        128, 128
    );

    REQUIRE( barrier.buffer == buffer.buffer );
    REQUIRE( barrier.srcStageMask == vk::PipelineStageFlagBits2::eTransfer );
    REQUIRE( barrier.srcAccessMask == vk::AccessFlags2{ vk::AccessFlagBits2::eTransferWrite} );
    REQUIRE( barrier.dstStageMask == vk::PipelineStageFlagBits2::eComputeShader );
    REQUIRE( barrier.dstAccessMask == vk::AccessFlags2{ vk::AccessFlagBits2::eShaderStorageRead} );
    REQUIRE( barrier.offset == 128 );
    REQUIRE( barrier.size == 128 );
}


TEST_CASE("create image memory barrier", "[synchronization]") {

    vktg::Image image;
    vktg::CreateImage( image, 256, 256, vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment);

    vk::ImageMemoryBarrier2 barrier = vktg::CreateImageMemoryBarrier(
        image.image, 
        vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
        vk::PipelineStageFlagBits2::eTopOfPipe, vk::AccessFlagBits2::eNone,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eColorAttachmentWrite
    );

    REQUIRE( barrier.image == image.image );
    REQUIRE( barrier.oldLayout == vk::ImageLayout::eUndefined );
    REQUIRE( barrier.newLayout == vk::ImageLayout::eColorAttachmentOptimal );
    REQUIRE( barrier.srcStageMask == vk::PipelineStageFlagBits2::eTopOfPipe );
    REQUIRE( barrier.srcAccessMask == vk::AccessFlags2{ vk::AccessFlagBits2::eNone} );
    REQUIRE( barrier.dstStageMask == vk::PipelineStageFlagBits2::eColorAttachmentOutput );
    REQUIRE( barrier.dstAccessMask == vk::AccessFlags2{ vk::AccessFlagBits2::eColorAttachmentWrite} );
}