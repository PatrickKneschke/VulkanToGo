
#pragma once


#include "vk_core.h"


namespace vktg
{
    

    /// @brief Holds the Vulkan swapchain, images and image views, along with all the data used to (re-)create it. Also contains flag to indicate if swapcahin needs recreating.
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


		/// @brief Access swapchain image width.
		/// @return Image width.
		uint32_t Width() { return extent.width; }
		/// @brief Access swapcahi mage height.
		/// @return Image height.
		uint32_t Height() { return extent.height; }
    };


    /// @brief Gathers all the data to create Vulkan swapchain and returns an uninitialized Swapchain object.
    /// @return Swapchain object.
    Swapchain PrepareSwapchain();

    /// @brief Creates the Vulkan swapchain, stores its images and creates the image views.
	//		   If given an already initialized swapchain, the old swapchain is destroyed and recreated with updated window size.
    /// @param swapchain Swapchain object created by PrepareSwapchain().
    void CreateSwapchain( Swapchain &swapchain);
	
	/// @brief Destroys given swapchain object along with its image views.
	/// @param swapchain Swapcahin to estroy.
	void DestroySwapchain( Swapchain &swapchain);
	
	/// @brief Destroys swapchain image views. Mostly for internal use in swapcahin recreation.
	/// @param swapchain Swapchain whos image view to destroy.
	void DestroySwapchainImageViews( Swapchain &swapchain);
    
	
	/// @brief Aquire next swapchain image. Will invalidate swapchain if it is out of date.
	/// @param swapchain Swapchain object holding Vulkan swapchain.
	/// @param readySemaphore Semaphore to siganl once image ahs been aquired.
	/// @param imageIndex Pointer to store index of next image.
	/// @return True is swapchain is still valid, false otherwise.
	bool NextSwapchainImage( Swapchain &swapchain, vk::Semaphore readySemaphore, uint32_t *imageIndex);

	/// @brief Presents swapcahin image. Will invalidate swapchain if it is out of data, suboptimal or if the window size has changed.
	/// @param swapchain Swapchain object holding Vulkan swapchain.
	/// @param waitSemaphore Semaphores to wait on before presenting.
	/// @param imageIndex Index of the swapchain image to present.
	/// @return True is swapchain is still valid, false otherwise.
	bool PresentImage( Swapchain &swapchain, vk::Semaphore *waitSemaphore, uint32_t *imageIndex);


} // namespace vktg
