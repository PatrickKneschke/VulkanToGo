#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    /// @brief Creates attachment for dynamic rendering. 
    ///        Intended for internal use only, you should call the Create*Attachment functions for color and depth attachments instead.
    /// @param view Image view of the attachment
    /// @param pClear Clear value of the attachment.
    /// @param layout Image layout of the attachment.
    /// @return Vulkan rendering attachment info.
    vk::RenderingAttachmentInfo CreateRenderingAttachment( vk::ImageView view, const vk::ClearValue* pClear = nullptr, vk::ImageLayout layout=vk::ImageLayout::eColorAttachmentOptimal);

    /// @brief Creates color attachment for dynamic rendering.
    /// @param view Image view of the attachment.
    /// @param pClear Clear color value for color attachment.
    /// @return Vulkan rendering attachment info.
    vk::RenderingAttachmentInfo CreateColorAttachment( vk::ImageView view, const vk::ClearValue* pClear = nullptr);
    
    /// @brief Creates depth-stencil attachment for dynamic rendering.
    /// @param view Image view of the attachment.
    /// @param pClear Clear depth-stencil value for color attachment.
    /// @return Vulkan rendering attachment info.
    vk::RenderingAttachmentInfo CreateDepthStencilAttachment( vk::ImageView view, const vk::ClearValue* pClear = nullptr);


    /// @brief Creates clear color value for rendering attachments.
    /// @param r Clear color red component.
    /// @param g Clear color grean component.
    /// @param b Clear color blue component.
    /// @param a Clear color alpha component.
    /// @return Vulkan clear value.
    vk::ClearValue CreateClearColorValue( float r, float g, float b, float a);

    /// @brief Creates clear depth-stencil value for rendering attachment.
    /// @param d Clear depth value.
    /// @param s Clear stencil value.
    /// @return Vulkan clear value.
    vk::ClearValue CreateClearDepthStencilValue( float d, float s);


    /// @brief Creates rendering info for dynamic rendering.
    /// @param renderExtent Extent of render area.
    /// @param colorAttachments List of color attachment.
    /// @param pDepthAttachment Pointer to depth(-stencil) attachent.
    /// @param pStencilAttachment Pointer to stencil attachment. For furture use as separate depth and stencil attachments are no supported yet.
    /// @return Vulkan rendering info
    vk::RenderingInfo CreateRenderingInfo( 
        vk::Extent2D renderExtent, std::span<vk::RenderingAttachmentInfo> colorAttachments, 
        const vk::RenderingAttachmentInfo* pDepthAttachment = nullptr, 
        const vk::RenderingAttachmentInfo* pStencilAttachment = nullptr
    );


    /// @brief Performs image layout transition by placing image barrier.
    /// @param cmd Command buffer to record pipeline barrier.
    /// @param image Image to change layout.
    /// @param oldLayout Old image layout.
    /// @param newLayout New image layout.
    /// @param srcStage Source pipeline stage for pipeline barrier.
    /// @param srcAccessMask Source memory access for pipeline barrier.
    /// @param dststage Destination pipeline stage for pipeline barrier.
    /// @param dstAccessMask Destination memory access for pipeline barrier,
    /// @param subResource Image subresource.
    void TransitionImageLayout( 
        vk::CommandBuffer cmd, vk::Image image, 
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccessMask, 
        vk::PipelineStageFlags2 dststage, vk::AccessFlags2 dstAccessMask,
        const vk::ImageSubresourceRange &subResource = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} 
    );

    
} // namespace vktg
