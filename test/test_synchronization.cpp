
#include <catch2/catch.hpp>

#include "../vulkantogo/synchronization.h"


TEST_CASE("create fence", "[synchronization]") {

    vk::Fence fence = vktg::CreateFence();
    vk::Result result = vktg::Device().getFenceStatus( fence);

    REQUIRE_FALSE( !fence);
    REQUIRE( result == vk::Result::eSuccess);

    vktg::DestroyFence( fence);
}


TEST_CASE("create semaphore", "[synchronization]") {

    vk::Semaphore semaphore = vktg::CreateSemaphore( vk::SemaphoreType::eTimeline);

    auto signalInfo = vk::SemaphoreSignalInfo{}
        .setSemaphore( semaphore )
        .setValue( 123);

    VK_CHECK( vktg::Device().signalSemaphore( &signalInfo) );

    uint64_t counter;
    VK_CHECK( vktg::Device().getSemaphoreCounterValue( semaphore, &counter) );

    REQUIRE_FALSE( !semaphore);
    REQUIRE( counter == 123);

    vktg::DestroySemaphore( semaphore);
}