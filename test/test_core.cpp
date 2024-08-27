
#include <catch2/catch.hpp>

#include "../vulkantogo/vk_core.h"


TEST_CASE("create instance", "[core]") {

    auto instance = vktg::Instance();

    REQUIRE_FALSE( !instance);
}