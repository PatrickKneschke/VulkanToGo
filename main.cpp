
#include "vulkantogo.h"

#include <iostream>


int main() {

    vktg::StartUp();


    // swapchain
    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    vktg::Image renderImage = vktg::CreateImage(
        1920, 1080, vk::Format::eR16G16B16A16Sfloat, 
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
    ); 


    vk::Semaphore renderSemaphore, presentSemaphore;
    auto semaphoreInfo = vk::SemaphoreCreateInfo{};
    VK_CHECK( vktg::Device().createSemaphore( &semaphoreInfo, nullptr, &renderSemaphore) );
    VK_CHECK( vktg::Device().createSemaphore( &semaphoreInfo, nullptr, &presentSemaphore) );
    vk::Fence renderFence;
    auto fenceInfo = vk::FenceCreateInfo{}.setFlags( vk::FenceCreateFlagBits::eSignaled);
    VK_CHECK( vktg::Device().createFence( &fenceInfo, nullptr, &renderFence) );


    // redner loop
    while (!glfwWindowShouldClose( vktg::Window())) 
    {
        uint32_t imageIndex;
        vktg::NextSwapchainImage( swapchain, renderSemaphore, &imageIndex);

        vktg::PresentImage( swapchain, &presentSemaphore, &imageIndex);
    }


    vktg::ShutDown();

    return 0;
}