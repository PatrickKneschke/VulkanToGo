
#include <catch2/catch.hpp>

#include "../vulkantogo/util/timer.h"


TEST_CASE("create timer", "[util, timer]") {

    vktg::Timer timer;

    REQUIRE( timer.TimeScale() == 1.f );
    REQUIRE( timer.DeltaTime() == 0.f );
    REQUIRE( timer.ElapsedTime() == 0.f );
    REQUIRE( timer.ElapsedUnscaledTime() == 0.f );
    REQUIRE( timer.ElapsedFrames() == 0 );
    REQUIRE_FALSE( timer.IsRunning() );
}


TEST_CASE("set time scale", "[util, timer]") {

    vktg::Timer timer;
    timer.TimeScale( 0.5f);

    REQUIRE( timer.TimeScale() == 0.5f );
}