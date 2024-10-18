#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    struct Buffer {

        vk::Buffer buffer;
        vk::BufferCreateInfo bufferInfo;

        vma::Allocation allocation;
        vma::AllocationCreateInfo allocationCreateInfo;
        vma::AllocationInfo allocationInfo;

        size_t Size() { return bufferInfo.size; }
        void* Data() { return allocationInfo.pMappedData; }
        vk::BufferUsageFlags Usage() { return bufferInfo.usage; }
        vma::MemoryUsage Memoryusage() { return allocationCreateInfo.usage; }
    };

    void CreateBuffer(
        Buffer &buffer,
        size_t bufferSize, vk::BufferUsageFlags bufferUsage, 
        vma::MemoryUsage memoryUsage = vma::MemoryUsage::eGpuOnly, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{},
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive, std::span<uint32_t>  queueFamilies = {}
    );

    void ResizeBuffer( Buffer &buffer, size_t newSize);

    void DestroyBuffer( const Buffer &buffer);

    void CreateStagingBuffer( Buffer &buffer, size_t bufferSize, void *data = nullptr);


    struct Image {

        vk::Image image;
        vk::ImageView imageView;
        vk::ImageAspectFlags imageAspect;
        vk::ImageCreateInfo imageInfo;

        vma::Allocation allocation;
        vma::AllocationCreateInfo allocationCreateInfo;;
        vma::AllocationInfo allocationInfo;

        uint32_t Width() { return imageInfo.extent.width; }
        uint32_t Height() { return imageInfo.extent.height; }
        vk::Format Format() { return imageInfo.format; }
        vk::ImageUsageFlags Usage() { return imageInfo.usage; }
        vma::MemoryUsage MemoryUsage() { return allocationCreateInfo.usage; }
        uint32_t MipLevels() { return imageInfo.mipLevels; }
        uint32_t Layers() { return imageInfo.arrayLayers; }
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

    void ResizeImage( Image &image, uint32_t newWidth, uint32_t newHeight);
    
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
