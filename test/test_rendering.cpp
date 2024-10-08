
#include <catch2/catch.hpp>

#include "../vulkantogo/rendering.h"
#include "../vulkantogo/storage.h"


TEST_CASE( "create rendering atachment info with default parameters", "[rendering]") {

    vktg::Image image;
    vktg::CreateImage( image, 256, 256, vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment);
    vk::RenderingAttachmentInfo attachment = vktg::CreateRenderingAttachment( image.imageView);

    REQUIRE( attachment.imageView == image.imageView );
    REQUIRE( attachment.imageLayout == vk::ImageLayout::eColorAttachmentOptimal );
    REQUIRE( attachment.loadOp == vk::AttachmentLoadOp::eLoad );

    vktg::DestroyImage( image);
}


TEST_CASE( "create rendering atachment info with clear color", "[rendering]") {

    vktg::Image image;
    vktg::CreateImage( image, 256, 256, vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
    vk::ClearValue clearValue = vk::ClearValue{}.setDepthStencil( {1.0f, 0} ); 
    vk::RenderingAttachmentInfo attachment = vktg::CreateRenderingAttachment( image.imageView, &clearValue, vk::ImageLayout::eDepthAttachmentOptimal);

    REQUIRE( attachment.imageView == image.imageView );
    REQUIRE( attachment.imageLayout == vk::ImageLayout::eDepthAttachmentOptimal );
    REQUIRE( attachment.loadOp == vk::AttachmentLoadOp::eClear );

    vktg::DestroyImage( image);
}


TEST_CASE( "create clear values", "[rednering]") {

    vk::ClearValue clearColor = vktg::CreateClearColorValue( 0.1f, 0.1f, 0.1f, 1.f);
    vk::ClearValue clearDepthStencil = vktg::CreateClearDepthStencilValue( 1.f, 0.f);

    REQUIRE( clearColor.color.float32 == std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f} );
    REQUIRE( clearDepthStencil.depthStencil.depth == 1.f );
    REQUIRE( clearDepthStencil.depthStencil.stencil == 0.f );
}


TEST_CASE( "create rendering info", "[rendering]") {

    vktg::Image colorImage1;
    vktg::CreateImage( colorImage1, 256, 256, vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment);
    vktg::Image colorImage2;
    vktg::CreateImage( colorImage2, 256, 256, vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment);
    vktg::Image depthImage;
    vktg::CreateImage( depthImage, 256, 256, vk::Format::eD24UnormS8Uint, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);

    vk::RenderingAttachmentInfo colorAttachments[2] = {
        vktg::CreateColorAttachment( colorImage1.imageView),
        vktg::CreateColorAttachment( colorImage2.imageView)
    };
     vk::RenderingAttachmentInfo depthStencilAttachment = vktg::CreateDepthStencilAttachment( depthImage.imageView);
    
    vk::RenderingInfo renderInfo = vktg::CreateRenderingInfo( vk::Extent2D{ colorImage1.imageInfo.extent.width, colorImage1.imageInfo.extent.height}, colorAttachments, &depthStencilAttachment);

    REQUIRE( renderInfo.layerCount == 1 );
    REQUIRE( renderInfo.colorAttachmentCount == 2 );
    REQUIRE( renderInfo.pColorAttachments == colorAttachments );
    REQUIRE( renderInfo.pDepthAttachment == &depthStencilAttachment );
    REQUIRE( renderInfo.pStencilAttachment == nullptr );

    vktg::DestroyImage( colorImage1);
    vktg::DestroyImage( colorImage2);
    vktg::DestroyImage( depthImage);
}