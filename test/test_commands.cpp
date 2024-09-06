
#include <catch2/catch.hpp>

#include "../vulkantogo/commands.h"


TEST_CASE("create command pool", "[commands]") {

    vk::CommandPool pool = vktg::CreateCommandPool( vktg::GraphicsQueueIndex());

    REQUIRE_FALSE( !pool);
}


TEST_CASE("create command buffer", "[commands]") {

    vk::CommandPool pool = vktg::CreateCommandPool( vktg::GraphicsQueueIndex());
    vk::CommandBuffer cmd = vktg::AllocateCommandBuffer( pool);

    REQUIRE_FALSE( !cmd);
}