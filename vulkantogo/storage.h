#pragma once


#include "vk_core.h"


namespace vktg
{


    struct Buffer {
        vk::Buffer buffer;
        const vk::BufferUsageFlags bufferUsage = vk::BufferUsageFlags{};
        const size_t bufferSize = 0;

        const vma::MemoryUsage memoryUsage = vma::MemoryUsage::eUnknown;
        vma::Allocation allocation;
        vma::AllocationInfo allocationInfo;        
    };

    Buffer CreateBuffer( size_t bufferSize, vk::BufferUsageFlags bufferUsage, vma::MemoryUsage memoryUsage = vma::MemoryUsage::eGpuOnly, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{});

    void DestroyBuffer( const Buffer &buffer);


    struct Image {
        vk::Image image;
        vk::ImageView imageView;
        const vk::ImageAspectFlags imageAspect;
        const vk::ImageCreateInfo imageInfo;

        const vma::MemoryUsage memoryUsage;
        vma::Allocation allocation;
    };

    Image CreateImage(
        uint32_t width, uint32_t height,
        vk::Format format, vk::ImageUsageFlags usage, 
        vk::ImageAspectFlags imageAspect = vk::ImageAspectFlagBits::eColor,
        uint32_t mipLevels = 1, uint32_t layers = 1, vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1,
        vma::MemoryUsage memoryUsage = vma::MemoryUsage::eGpuOnly, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{},
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive, uint32_t queueFamilyCount = 0, const uint32_t* pQueueFamilies = nullptr
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
