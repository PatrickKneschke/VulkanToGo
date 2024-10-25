#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    /// @brief Copies single buffer region from source buffer to destination buffer.
    /// @param cmd Command buffer to record copy command.
    /// @param srcBuffer Source buffer.
    /// @param dstBuffer Destination buffer.
    /// @param copySize Size of copy region.
    /// @param srcOffset Offset of source buffer region,
    /// @param dstOffset Offset of destination buffer region.
    void CopyBuffer( 
        vk::CommandBuffer cmd,
        vk::Buffer srcBuffer, vk::Buffer dstBuffer,
        size_t copySize, size_t srcOffset = 0, size_t dstOffset = 0);

    /// @brief Copies multiple buffer regions from source buffer to destination buffer.
    /// @param cmd Command buffer to record copy command.
    /// @param srcBuffer Source buffer.
    /// @param dstBuffer Destination buffer.
    /// @param regions List of buffer regions to copy.
    void CopyBuffer( 
        vk::CommandBuffer cmd,
        vk::Buffer srcBuffer, vk::Buffer dstBuffer,
        std::span<vk::BufferCopy2> regions);

    
    /// @brief Copies image region from source image to destination image,
    /// @param cmd Command buffer to record copy command.
    /// @param srcImage Source image.
    /// @param dstImage Destination image.
    /// @param srcRegion Source image region to copy from.
    /// @param dstRegion Destination image region to copy to.
    /// @param filter Filter to apply if source/destination region sizes do not match.
    /// @param srcSubresourse Source image subresource.
    /// @param dstSubresourse Destination image subresource.
    void CopyImage( 
        vk::CommandBuffer cmd, 
        vk::Image srcImage, vk::Image dstImage, 
        vk::Rect2D srcRegion, vk::Rect2D dstRegion,
        vk::Filter filter = vk::Filter::eLinear,
        vk::ImageSubresourceLayers srcSubresourse = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
        vk::ImageSubresourceLayers dstSubresourse = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );


    /// @brief Copies a region from source buffer to destination image. Copy region size is image region width x height.
    /// @param cmd Command buffer to record copy command.
    /// @param srcBuffer Source buffer.
    /// @param dstImage Destination image.
    /// @param bufferOffset Offset of source buffer region.
    /// @param imgWidth Width of dstination image region.
    /// @param imgHeight Height of destinatio image region.
    /// @param imgOffset Offset of destination image region.
    /// @param imgSubresource Destination image subresource.
    void CopyBufferToImage(
        vk::CommandBuffer cmd,
        vk::Buffer srcBuffer, vk::Image dstImage,
        size_t bufferOffset, uint32_t imgWidth, uint32_t imgHeight,
        vk::Offset3D imgOffset = vk::Offset3D{0, 0, 0},
        vk::ImageSubresourceLayers imgSubresource = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );


    /// @brief Copies image region from source image to destination buffer.
    /// @param cmd Command buffer to record copy command.
    /// @param srcImage Source image,
    /// @param dstBuffer Destinationbuffer.
    /// @param bufferOffset Destination buffer region offset.
    /// @param imgWidth Width of source image region.
    /// @param imgHeight Height of source image region.
    /// @param imgOffset Offset of source image region.
    /// @param imgSubresource Source image subresource.
    void CopyImageToBuffer(
        vk::CommandBuffer cmd,
        vk::Image srcImage, vk::Buffer dstBuffer, 
        size_t bufferOffset, uint32_t imgWidth, uint32_t imgHeight,
        vk::Offset3D imgOffset = vk::Offset3D{0, 0, 0},
        vk::ImageSubresourceLayers imgSubresource = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );


    /// @brief Uploads buffer data to GPU. Intended for use on separate thread, since it will wait for completion.
    /// @param srcData Pointer to source data location.
    /// @param dstBuffer Destination buffer.
    /// @param size Size of data to copy.
    /// @param offset Offset of buffer region to copy into.
    void UploadBufferData( void* srcData, vk::Buffer dstBuffer, size_t size, size_t offset);

    /// @brief Uploads image data to GPU. Intended for use on separate thread, since it will wait for completion.
    /// @param srcData Pointer to source data location.
    /// @param dstImage Destination image.
    /// @param width Width of destination image region.
    /// @param height Height of destination image region.
    /// @param imgOffset Offset of destination image region.
    /// @param imgSubresource Destination image subresource
    void UploadImageData( 
        void* srcData, vk::Image dstImage, 
        uint32_t width, uint32_t height, vk::Offset3D imgOffset = vk::Offset3D{0, 0, 0},
        vk::ImageSubresourceLayers imgSubresource = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );

    
} // namespace vktg