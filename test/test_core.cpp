
#include <catch2/catch.hpp>

#include "../vulkantogo/vk_core.h"


TEST_CASE("startup core", "[core]") {

    vktg::StartUp( 1920, 1080, "Title");
    
    REQUIRE_FALSE( !vktg::Instance());
    REQUIRE_FALSE( !vktg::DebugMessenger());
}
