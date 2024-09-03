
#include <catch2/catch.hpp>

#include "../vulkantogo/storage.h"


TEST_CASE("unitialized buffer", "[storage]") {

    vktg::Buffer buffer;

    REQUIRE( !buffer.buffer);
    REQUIRE( buffer.bufferSize == 0);
    REQUIRE( buffer.bufferUsage == vk::BufferUsageFlags{});
    REQUIRE( buffer.memoryUsage == vma::MemoryUsage::eUnknown);
    REQUIRE( !buffer.allocation);
}


TEST_CASE("create buffer", "[storage]") {

    vktg::Buffer buffer = vktg::CreateBuffer( 256, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);
    
    REQUIRE_FALSE( !buffer.buffer);
    REQUIRE( buffer.bufferSize == 256);
    REQUIRE( buffer.bufferUsage == vk::BufferUsageFlagBits::eUniformBuffer);
    REQUIRE( buffer.memoryUsage == vma::MemoryUsage::eCpuOnly);
    REQUIRE_FALSE( !buffer.allocation);
    REQUIRE( buffer.allocationInfo.pMappedData != nullptr);
}


TEST_CASE("create image", "[storage]") {

    vktg::Image image = vktg::CreateImage( 
        256, 256, vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, 8);

    REQUIRE_FALSE( !image.image);
    REQUIRE_FALSE( !image.imageView);
    REQUIRE_FALSE( !image.allocation);
    REQUIRE( image.imageInfo.extent == vk::Extent3D{256, 256, 1});
    REQUIRE( image.imageInfo.mipLevels == 8);
    REQUIRE( image.imageInfo.arrayLayers == 1);
    REQUIRE( image.imageInfo.format == vk::Format::eD32Sfloat);
    REQUIRE( image.imageInfo.imageType == vk::ImageType::e2D);
}

