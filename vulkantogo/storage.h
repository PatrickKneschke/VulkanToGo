#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    struct Buffer {
        vk::Buffer buffer;
        vk::BufferUsageFlags bufferUsage = vk::BufferUsageFlags{};
        size_t bufferSize = 0;

        vma::MemoryUsage memoryUsage = vma::MemoryUsage::eUnknown;
        vma::Allocation allocation;
        vma::AllocationInfo allocationInfo;        
    };

    void CreateBuffer(
        Buffer &buffer,
        size_t bufferSize, vk::BufferUsageFlags bufferUsage, 
        vma::MemoryUsage memoryUsage = vma::MemoryUsage::eGpuOnly, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{},
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive, std::span<uint32_t>  queueFamilies = {}
    );

    void CreateStagingBuffer( Buffer &buffer, size_t bufferSize, void *data = nullptr);

    void DestroyBuffer( const Buffer &buffer);


    struct Image {
        vk::Image image;
        vk::ImageView imageView;
        vk::ImageAspectFlags imageAspect;
        vk::ImageCreateInfo imageInfo;

        vma::MemoryUsage memoryUsage;
        vma::Allocation allocation;
        vma::AllocationInfo allocationInfo;

        uint32_t Width() { return imageInfo.extent.width; }
        uint32_t Height() { return imageInfo.extent.height; }
        vk::Format Format() { return imageInfo.format; }
    };

    void CreateImage(
        Image &image,
        uint32_t width, uint32_t height,
        vk::Format format, vk::ImageUsageFlags usage, 
        vk::ImageAspectFlags imageAspect = vk::ImageAspectFlagBits::eColor,
        uint32_t mipLevels = 1, uint32_t layers = 1, vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1,
        vma::MemoryUsage memoryUsage = vma::MemoryUsage::eGpuOnly, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{},
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive, std::span<uint32_t>  queueFamilies = {}
    );
    
    void DestroyImage( const Image &image);

    vk::ImageView CreateImageView(
		vk::Image image, 
        vk::Format format,
		vk::ImageAspectFlags aspectFlags,
        uint32_t baseMipLevel = 0,
		uint32_t mipLevels = 1,
        uint32_t baseLayer = 0,
        uint32_t layerCount = 1,
        vk::ImageViewType viewType = vk::ImageViewType::e2D
	);

    void DestroyImageView( vk::ImageView imageView);
    

} // namespace vktg
