
#include "transfer.h"


namespace vktg
{


    void CopyBuffer( vk::CommandBuffer cmd, vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t copySize, size_t srcOffset, size_t dstOffset) {

        auto copyRegion = vk::BufferCopy2{}
            .setSize( copySize )
            .setSrcOffset( srcOffset )
            .setDstOffset( dstOffset );

        auto copyInfo = vk::CopyBufferInfo2{}
            .setSrcBuffer( srcBuffer )
            .setDstBuffer( dstBuffer )
            .setRegionCount( 1 )
            .setPRegions( &copyRegion );

        cmd.copyBuffer2( &copyInfo);
    }


    void CopyBuffer(vk::CommandBuffer cmd, vk::Buffer srcBuffer, vk::Buffer dstBuffer, std::span<vk::BufferCopy2> regions) {

        auto copyInfo = vk::CopyBufferInfo2{}
            .setSrcBuffer( srcBuffer )
            .setDstBuffer( dstBuffer )
            .setRegionCount( (uint32_t)regions.size() )
            .setPRegions( regions.data() );

        cmd.copyBuffer2( &copyInfo);
    }

    
    void CopyImage(vk::CommandBuffer cmd, vk::Image srcImage, vk::Image dstImage, vk::Rect2D srcRegion, vk::Rect2D dstRegion, vk::Filter filter, vk::ImageSubresourceLayers srcSubresourse, vk::ImageSubresourceLayers dstSubresourse) {

        auto blitRegion = vk::ImageBlit2{}
            .setSrcOffsets( {vk::Offset3D{srcRegion.offset.x, srcRegion.offset.y, 0}, vk::Offset3D{(int)srcRegion.extent.width, (int)srcRegion.extent.height, 1}} )
            .setDstOffsets( {vk::Offset3D{dstRegion.offset.x, dstRegion.offset.y, 0}, vk::Offset3D{(int)dstRegion.extent.width, (int)dstRegion.extent.height, 1}} )
            .setSrcSubresource( srcSubresourse )
            .setDstSubresource( dstSubresourse );

        auto blitInfo = vk::BlitImageInfo2{}
            .setSrcImage( srcImage )
            .setSrcImageLayout( vk::ImageLayout::eTransferSrcOptimal )
            .setDstImage( dstImage )
            .setDstImageLayout( vk::ImageLayout::eTransferDstOptimal )
            .setFilter( filter )
            .setRegionCount( 1 )
            .setPRegions( &blitRegion );

        cmd.blitImage2( &blitInfo);
    }

    
    void CopyBufferToImage(vk::CommandBuffer cmd, vk::Buffer srcBuffer, vk::Image dstImage, size_t bufferOffset, uint32_t imgWidth, uint32_t imgHeight, vk::Offset3D imgOffset, vk::ImageSubresourceLayers imgSubresource) {

        auto copyRegion = vk::BufferImageCopy2{}
            .setBufferOffset( bufferOffset )
            .setImageOffset( imgOffset )
            .setImageExtent( vk::Extent3D{imgWidth, imgHeight, 1} )
            .setImageSubresource( imgSubresource);

        auto copyInfo = vk::CopyBufferToImageInfo2{}
            .setSrcBuffer( srcBuffer )
            .setDstImage( dstImage )
            .setDstImageLayout( vk::ImageLayout::eTransferDstOptimal )
            .setRegionCount( 1 )
            .setPRegions( &copyRegion );        

        cmd.copyBufferToImage2( &copyInfo);
    }


    void CopyImageToBuffer(vk::CommandBuffer cmd, vk::Image srcImage, vk::Buffer dstBuffer, size_t bufferOffset, uint32_t imgWidth, uint32_t imgHeight, vk::Offset3D imgOffset, vk::ImageSubresourceLayers imgSubresource) {

        auto copyRegion = vk::BufferImageCopy2{}
            .setBufferOffset( bufferOffset)
            .setImageOffset( imgOffset )
            .setImageExtent( vk::Extent3D{imgWidth, imgHeight, 1} );

        auto copyInfo = vk::CopyImageToBufferInfo2{}
            .setSrcImage( srcImage )
            .setSrcImageLayout( vk::ImageLayout::eTransferSrcOptimal )
            .setDstBuffer( dstBuffer )
            .setRegionCount( 1 )
            .setPRegions( &copyRegion );

        cmd.copyImageToBuffer2( &copyInfo);
    }


} // namespace vktg