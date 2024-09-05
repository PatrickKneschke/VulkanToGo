
#include <catch2/catch.hpp>

#include "../vulkantogo/swapchain.h"


TEST_CASE("prepare swapchain", "[swapchain]") {

    vktg::Swapchain swapchain = vktg::PrepareSwapchain();

    REQUIRE( !swapchain.swapchain);
    REQUIRE( swapchain.imageCount > 0);
    REQUIRE( swapchain.images.empty());
    REQUIRE( swapchain.imageViews.empty());
    REQUIRE( swapchain.imageFormat == vk::Format::eB8G8R8A8Srgb);
    REQUIRE( swapchain.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
#ifdef NDEBUG
    REQUIRE( swapchain.presentMode == vk::PresentModeKHR::eFifo);
#else
    REQUIRE( swapchain.presentMode == vk::PresentModeKHR::eImmediate);
#endif
    REQUIRE( swapchain.extent.width == 0);
    REQUIRE( swapchain.extent.height == 0);
    REQUIRE( !swapchain.isValid);
}


TEST_CASE("create swapchain", "[swapchain]") {

    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    REQUIRE_FALSE( !swapchain.swapchain);
    REQUIRE( swapchain.imageCount > 0);
    REQUIRE( swapchain.images.size() > 0);
    REQUIRE( swapchain.imageViews.size() > 0);
    REQUIRE( swapchain.imageFormat == vk::Format::eB8G8R8A8Srgb);
    REQUIRE( swapchain.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
#ifdef NDEBUG
    REQUIRE( swapchain.presentMode == vk::PresentModeKHR::eFifo);
#else
    REQUIRE( swapchain.presentMode == vk::PresentModeKHR::eImmediate);
#endif
    REQUIRE( swapchain.extent.width > 0);
    REQUIRE( swapchain.extent.height > 0);
    REQUIRE( swapchain.isValid);

    vktg::DestroySwapchain( swapchain);
}


TEST_CASE("get next swapchain image", "[swapchain]") {

    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    vk::Semaphore renderSemaphore;
    auto semaphoreInfo = vk::SemaphoreCreateInfo{};
    VK_CHECK( vktg::Device().createSemaphore( &semaphoreInfo, nullptr, &renderSemaphore) );
    uint32_t imageIndex;
    vktg::NextSwapchainImage( swapchain, renderSemaphore, &imageIndex);

    REQUIRE( imageIndex == 0);
    
    vktg::NextSwapchainImage( swapchain, renderSemaphore, &imageIndex);

    REQUIRE( imageIndex == 1);
    

    vktg::DestroySwapchain( swapchain);
}


TEST_CASE("swapchain recreation", "[swapchain]") {

    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);
    auto oldSwapchain = swapchain;
    vktg::CreateSwapchain( swapchain);

    REQUIRE( swapchain.swapchain != oldSwapchain.swapchain);
    for (uint32_t i=0; i<swapchain.imageCount; i++)
    {
        REQUIRE( swapchain.imageViews[i] != oldSwapchain.imageViews[i]);
    }

    vktg::DestroySwapchain( swapchain);
}