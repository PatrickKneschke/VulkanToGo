
#pragma once


#include "vk_core.h"


namespace vktg
{
    

    struct Swapchain {
        vk::SwapchainKHR swapchain;
		uint32_t imageCount;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		vk::Format imageFormat;
		vk::ColorSpaceKHR colorSpace;
		vk::PresentModeKHR presentMode;
		vk::Extent2D extent;
		bool isValid;
    };


    Swapchain PrepareSwapchain();
    void CreateSwapchain( Swapchain &swapchain);
	void DestroySwapchain( Swapchain &swapchain);
	void DestroySwapchainImageViews( Swapchain &swapchain);
    bool NextSwapchainImage( Swapchain &swapchain, vk::Semaphore readySemaphore, uint32_t *imageIndex);
    bool PresentImage( Swapchain &swapchain, vk::Semaphore *waitSemaphore, uint32_t *imageIndex);


} // namespace vktg
