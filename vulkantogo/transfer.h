#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    void CopyBuffer( 
        vk::CommandBuffer cmd,
        vk::Buffer srcBuffer, vk::Buffer dstBuffer,
        size_t copySize, size_t srcOffset = 0, size_t dstOffset = 0);

    void CopyBuffer( 
        vk::CommandBuffer cmd,
        vk::Buffer srcBuffer, vk::Buffer dstBuffer,
        std::span<vk::BufferCopy2> regions);

    
    void CopyImage( 
        vk::CommandBuffer cmd, 
        vk::Image srcImage, vk::Image dstImage, 
        vk::Rect2D srcRegion, vk::Rect2D dstRegion,
        vk::Filter filter = vk::Filter::eLinear,
        vk::ImageSubresourceLayers srcSubresourse = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
        vk::ImageSubresourceLayers dstSubresourse = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );


    void CopyBufferToImage(
        vk::CommandBuffer cmd,
        vk::Buffer srcBuffer, vk::Image dstImage,
        size_t bufferOffset, uint32_t imgWidth, uint32_t imgHeight,
        vk::Offset3D imgOffset = vk::Offset3D{0, 0, 0},
        vk::ImageSubresourceLayers imgSubresource = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );


    void CopyImageToBuffer(
        vk::CommandBuffer cmd,
        vk::Image srcImage, vk::Buffer dstBuffer, 
        size_t bufferOffset, uint32_t imgWidth, uint32_t imgHeight,
        vk::Offset3D imgOffset = vk::Offset3D{0, 0, 0},
        vk::ImageSubresourceLayers imgSubresource = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );


    void UploadBufferData( 
        vk::CommandBuffer cmd, 
        void* srcData, vk::Buffer dstBuffer, size_t size, size_t offset
    );

    void UploadImageData( 
        vk::CommandBuffer cmd, 
        void* srcData, vk::Image dstImage, 
        uint32_t width, uint32_t height, vk::Offset3D imgOffset = vk::Offset3D{0, 0, 0},
        vk::ImageSubresourceLayers imgSubresource = vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1}
    );

    
} // namespace vktg