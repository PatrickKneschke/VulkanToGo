
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


    vk::Fence renderFence = vktg::CreateFence();
    vk::Semaphore renderSemaphore = vktg::CreateSemaphore();
    vk::Semaphore presentSemaphore = vktg::CreateSemaphore();


    // redner loop
    while (!glfwWindowShouldClose( vktg::Window())) 
    {
        uint32_t imageIndex;
        vktg::NextSwapchainImage( swapchain, renderSemaphore, &imageIndex);

        vktg::PresentImage( swapchain, &presentSemaphore, &imageIndex);
    }


    vktg::DestroyFence( renderFence);
    vktg::DestroySemaphore( renderSemaphore);
    vktg::DestroySemaphore( presentSemaphore);


    vktg::ShutDown();

    return 0;
}