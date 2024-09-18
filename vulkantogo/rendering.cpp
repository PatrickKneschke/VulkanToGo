
#include "rendering.h"


namespace vktg
{


    vk::RenderingAttachmentInfo CreateRenderingAttachment( vk::ImageView view, vk::ClearValue *pClear, vk::ImageLayout layout) {

        auto attachmentInfo = vk::RenderingAttachmentInfo{}
            .setImageView( view )
            .setImageLayout( layout );
        if (pClear == nullptr)
        {
            attachmentInfo
                .setClearValue( vk::ClearValue{} )
                .setLoadOp( vk::AttachmentLoadOp::eLoad );
        }
        else 
        {
            attachmentInfo
                .setClearValue( *pClear )
                .setLoadOp( vk::AttachmentLoadOp::eClear );
        }

        return attachmentInfo;
    }

        
    vk::RenderingInfo CreateRenderingInfo(vk::Extent2D renderExtent, std::span<vk::RenderingAttachmentInfo> colorAttachments, vk::RenderingAttachmentInfo *pDepthAttachment, vk::RenderingAttachmentInfo *pStencilAttachment) {

        auto renderingInfo = vk::RenderingInfo{}
            .setRenderArea( vk::Rect2D{ vk::Offset2D{0, 0}, renderExtent})
            .setLayerCount( 1 )
            .setColorAttachmentCount( (uint32_t)colorAttachments.size() )
            .setPColorAttachments( colorAttachments.data() )
            .setPDepthAttachment( pDepthAttachment )
            .setPStencilAttachment( pStencilAttachment );

        return renderingInfo;
    }


} // namespace vktg
