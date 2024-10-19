
#include "swapchain.h"
#include "storage.h"


namespace vktg
{


    Swapchain PrepareSwapchain() {

        Swapchain swapchain;

        auto capabilities = Gpu().getSurfaceCapabilitiesKHR( Surface() );
        auto formats      = Gpu().getSurfaceFormatsKHR( Surface() );
        
        // find image count
        swapchain.imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0)
        {
            swapchain.imageCount = std::min( swapchain.imageCount, capabilities.maxImageCount);
        }
        
        // find image format and color space, default to first one available
        swapchain.imageFormat = formats[0].format;
        swapchain.colorSpace  = formats[0].colorSpace;
        for (const auto format : formats) 
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
            {
                swapchain.imageFormat = format.format;
                swapchain.colorSpace  = format.colorSpace;
                break;
            }
        }

        // find present mode
#ifdef NDEBUG
        swapchain.presentMode = vk::PresentModeKHR::eFifo;
#else
        swapchain.presentMode = vk::PresentModeKHR::eImmediate;
#endif

        swapchain.isValid = false;

        return swapchain;
    }


    void CreateSwapchain( Swapchain &swapchain) {

        // in case of swapchain recreation
        vk::SwapchainKHR oldSwapchain;
        if (swapchain.swapchain)
        {
            oldSwapchain = swapchain.swapchain;
            DestroySwapchainImageViews( swapchain);
        }

        // update swapchain extent to current window size
        swapchain.extent = Gpu().getSurfaceCapabilitiesKHR( Surface()).currentExtent;
        
        auto createInfo = vk::SwapchainCreateInfoKHR{}
            .setSurface( Surface() )
            .setMinImageCount( swapchain.imageCount )
            .setImageFormat( swapchain.imageFormat )
            .setImageColorSpace( swapchain.colorSpace )
            .setPresentMode( swapchain.presentMode )
            .setImageExtent( swapchain.extent )
            .setImageArrayLayers( 1 )
            .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
            .setPreTransform( vk::SurfaceTransformFlagBitsKHR::eIdentity )
            .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
            .setClipped( VK_TRUE )
            .setImageSharingMode( vk::SharingMode::eExclusive )
            .setOldSwapchain( oldSwapchain );
        
        
        VK_CHECK( Device().createSwapchainKHR( &createInfo, nullptr, &swapchain.swapchain) );	
        swapchain.isValid = true;

        // create swapchain images
        swapchain.images = Device().getSwapchainImagesKHR( swapchain.swapchain);
        swapchain.imageViews.resize( swapchain.imageCount);
        for (uint32_t i = 0; i < swapchain.imageCount; i++)
        {
            swapchain.imageViews[i] = CreateImageView( swapchain.images[i], swapchain.imageFormat, vk::ImageAspectFlagBits::eColor);
        }

        //destroy old swapchain
        if (oldSwapchain)
        {
            Device().destroySwapchainKHR( oldSwapchain);
        }
    }


    void DestroySwapchain( Swapchain &swapchain) {

        if (!swapchain.swapchain)
        {
            return;
        }

        DestroySwapchainImageViews( swapchain);
        Device().destroySwapchainKHR( swapchain.swapchain);

        swapchain.images.clear();
        swapchain.isValid = false;
    }

    
    void DestroySwapchainImageViews( Swapchain &swapchain) {

        for (auto imageView : swapchain.imageViews)
        {
            Device().destroyImageView( imageView);
        }
        swapchain.imageViews.clear();
    }


    bool NextSwapchainImage( Swapchain &swapchain, vk::Semaphore readySemaphore, uint32_t *imageIndex) {

        vk::Result result;
        try
        {
            result = Device().acquireNextImageKHR( swapchain.swapchain, UINT64_MAX, readySemaphore, nullptr, imageIndex);
        }
        catch (...)

        {
            if (result == vk::Result::eErrorOutOfDateKHR)
            {
                swapchain.isValid = false;
            }
            else
            {
                VK_CHECK( result );
            }
        }

        return swapchain.isValid;
    }


    bool PresentImage( Swapchain &swapchain, vk::Semaphore *waitSemaphore, uint32_t *imageIndex) {

        auto presentInfo = vk::PresentInfoKHR{}
            .setWaitSemaphoreCount( 1 )
            .setPWaitSemaphores( waitSemaphore )
            .setSwapchainCount( 1 )
            .setPSwapchains( &swapchain.swapchain )
            .setPImageIndices( imageIndex );
	
        vk::Result result;
        try
        {
            result = vktg::GraphicsQueue().presentKHR( presentInfo);
        }
        catch (...)
        {
            if (result == vk::Result::eErrorOutOfDateKHR  ||  result == vk::Result::eSuboptimalKHR  ||  swapchain.extent != vktg::Gpu().getSurfaceCapabilitiesKHR( vktg::Surface()).currentExtent )
            {
                swapchain.isValid = false;
            }
            else
            {
                VK_CHECK( result );
            }
        }

        return swapchain.isValid;
    }


} // namespace vktg