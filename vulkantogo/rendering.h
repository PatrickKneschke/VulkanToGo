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

    
} // namespace vktg
