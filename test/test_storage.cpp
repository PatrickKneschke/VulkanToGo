
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
