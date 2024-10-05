
#include <catch2/catch.hpp>

#include "../vulkantogo/submit_context.h"


TEST_CASE("create/destroy submit context", "[submit_context]") {

    vktg::SubmitContext context = vktg::CreateSubmitContext( vktg::QueueType::GRAPHICS);

    REQUIRE( context.queue == vktg::GraphicsQueue() );
    REQUIRE_FALSE( !context.cmdPool );
    REQUIRE_FALSE( !context.cmd );
    REQUIRE_FALSE( !context.fence );

    vk::Result result = vktg::Device().getFenceStatus( context.fence);
    
    REQUIRE( result == vk::Result::eNotReady);

    vktg::DestroySubmitContext( context);
}