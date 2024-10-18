
#include <catch2/catch.hpp>

#include "../vulkantogo/storage.h"


TEST_CASE("unitialized buffer", "[storage]") {

    vktg::Buffer buffer;

    REQUIRE( !buffer.buffer);
    REQUIRE( buffer.Size() == 0);
    REQUIRE( buffer.Usage() == vk::BufferUsageFlags{});
    REQUIRE( buffer.Memoryusage() == vma::MemoryUsage::eUnknown);
    REQUIRE( !buffer.allocation);
}


TEST_CASE("create buffer", "[storage]") {

    vktg::Buffer buffer;
    vktg::CreateBuffer( buffer, 256, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);
    
    REQUIRE_FALSE( !buffer.buffer);
    REQUIRE( buffer.Size() == 256);
    REQUIRE( buffer.Usage() == vk::BufferUsageFlagBits::eUniformBuffer);
    REQUIRE( buffer.Memoryusage() == vma::MemoryUsage::eCpuOnly);
    REQUIRE_FALSE( !buffer.allocation);
    REQUIRE( buffer.Data() != nullptr);

    vktg::DestroyBuffer( buffer);
}


TEST_CASE("resize buffer", "[storage]") {

    vktg::Buffer buffer;
    vktg::CreateBuffer( buffer, 256, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuOnly, vma::AllocationCreateFlagBits::eMapped);
    vktg::ResizeBuffer( buffer, 512);

    REQUIRE_FALSE( !buffer.buffer);
    REQUIRE( buffer.Size() == 512);
    REQUIRE( buffer.Usage() == vk::BufferUsageFlagBits::eUniformBuffer);
    REQUIRE( buffer.Memoryusage() == vma::MemoryUsage::eCpuOnly);
    REQUIRE_FALSE( !buffer.allocation);
    REQUIRE( buffer.Data() != nullptr);

    vktg::DestroyBuffer( buffer);
}


TEST_CASE("create staging buffer", "[storage]") {

    float bufferData[] = {1.f, 2.f, 3.f, 4.f};
    vktg::Buffer buffer;
    vktg::CreateStagingBuffer( buffer, sizeof( bufferData), bufferData);
    
    REQUIRE_FALSE( !buffer.buffer);
    REQUIRE( buffer.Size() == sizeof( bufferData));
    REQUIRE( buffer.Usage() == vk::BufferUsageFlagBits::eTransferSrc);
    REQUIRE( buffer.Memoryusage() == vma::MemoryUsage::eCpuOnly);
    REQUIRE_FALSE( !buffer.allocation);
    REQUIRE( buffer.Data() != nullptr);
 
    vktg::DestroyBuffer( buffer);
}


TEST_CASE("create image", "[storage]") {

    vktg::Image image;
    vktg::CreateImage( image, 256, 256, vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, 8);

    REQUIRE_FALSE( !image.image);
    REQUIRE_FALSE( !image.imageView);
    REQUIRE_FALSE( !image.allocation);
    REQUIRE( image.imageInfo.extent == vk::Extent3D{256, 256, 1});
    REQUIRE( image.imageInfo.mipLevels == 8);
    REQUIRE( image.imageInfo.arrayLayers == 1);
    REQUIRE( image.imageInfo.format == vk::Format::eD32Sfloat);
    REQUIRE( image.imageInfo.imageType == vk::ImageType::e2D);
 
    vktg::DestroyImage( image);
}

