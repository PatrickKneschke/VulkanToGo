
#include <catch2/catch.hpp>

#include "../vulkantogo/vk_core.h"


TEST_CASE("startup core", "[core]") {

    vktg::StartUp();

    REQUIRE_FALSE( !vktg::Window());
    REQUIRE_FALSE( !vktg::Instance());
    REQUIRE_FALSE( !vktg::DebugMessenger());
    REQUIRE_FALSE( !vktg::Surface());
    REQUIRE_FALSE( !vktg::Gpu());
    REQUIRE_FALSE( !vktg::Device());
    
    REQUIRE( vktg::GraphicsQueueIndex() >= 0);
    REQUIRE_FALSE( !vktg::GraphicsQueue());
    REQUIRE( vktg::ComputeQueueIndex() >= 0);
    REQUIRE_FALSE( !vktg::ComputeQueue());
    REQUIRE( vktg::TransferQueueIndex() >= 0);
    REQUIRE_FALSE( !vktg::TransferQueue());

    REQUIRE_FALSE( !vktg::Allocator());

    vktg::ShutDown();
}
