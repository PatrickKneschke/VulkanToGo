#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    vk::RenderingAttachmentInfo CreateRenderingAttachment( 
        vk::ImageView view, vk::ClearValue* pClear = nullptr, 
        vk::ImageLayout layout=vk::ImageLayout::eColorAttachmentOptimal
    );

    vk::RenderingInfo CreateRenderingInfo( 
        vk::Extent2D renderExtent, std::span<vk::RenderingAttachmentInfo> colorAttachments, 
        vk::RenderingAttachmentInfo* pDepthAttachment = nullptr, 
        vk::RenderingAttachmentInfo* pStencilAttachment = nullptr
    );


    void TransitionImageLayout( 
        vk::CommandBuffer cmd, vk::Image image, 
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::AccessFlagBits2 srcAccessMask, vk::AccessFlagBits2 dstAccessMask,
        vk::PipelineStageFlagBits2 srcStage, vk::PipelineStageFlagBits2 dststage,
        vk::ImageSubresourceRange subResource = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} 
    );

    
} // namespace vktg
