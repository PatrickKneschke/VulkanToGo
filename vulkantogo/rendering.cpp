
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

    
    void TransitionImageLayout( vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::PipelineStageFlags2 srcStage, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dststage, vk::AccessFlags2 dstAccessMask, vk::ImageSubresourceRange subResource) {
    
        auto barrier = vk::ImageMemoryBarrier2{}
            .setImage( image )
            .setOldLayout( oldLayout ) 
            .setNewLayout( newLayout )
            .setSrcStageMask( srcStage )
            .setSrcAccessMask( srcAccessMask )
            .setDstStageMask( dststage )
            .setDstAccessMask( dstAccessMask )
            .setSubresourceRange( subResource )
            .setSrcQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
            .setDstQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED );

        auto dependencyInfo = vk::DependencyInfo{}
            .setImageMemoryBarrierCount( 1 )
            .setPImageMemoryBarriers( &barrier );

        cmd.pipelineBarrier2( &dependencyInfo);
    }


} // namespace vktg
