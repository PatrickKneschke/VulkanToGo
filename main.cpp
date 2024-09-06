
#include "vulkantogo.h"

#include <iostream>


int main() {

    vktg::StartUp();


    // deletion stack
    vktg::DeletionStack deletionStack = vktg::DeletionStack{};

    // swapchain
    vktg::Swapchain swapchain = vktg::PrepareSwapchain();
    vktg::CreateSwapchain( swapchain);

    vktg::Image renderImage = vktg::CreateImage(
        1920, 1080, vk::Format::eR16G16B16A16Sfloat, 
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
    ); 


    // synchronization objects
    vk::Fence renderFence = vktg::CreateFence();
    vk::Semaphore renderSemaphore = vktg::CreateSemaphore();
    vk::Semaphore presentSemaphore = vktg::CreateSemaphore();

    deletionStack.Push( [=](){
        vktg::DestroyFence( renderFence);
    });
    deletionStack.Push( [=](){
        vktg::DestroySemaphore( renderSemaphore);
    });
    deletionStack.Push( [=](){
        vktg::DestroySemaphore( presentSemaphore);
    });


    // redner loop
    while (!glfwWindowShouldClose( vktg::Window())) 
    {
        uint32_t imageIndex;
        vktg::NextSwapchainImage( swapchain, renderSemaphore, &imageIndex);

        vktg::PresentImage( swapchain, &presentSemaphore, &imageIndex);
    }


    // cleanup
    deletionStack.Flush();

    vktg::DestroySwapchain( swapchain);

    vktg::ShutDown();

    return 0;
}