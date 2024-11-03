#pragma once


#include "vk_core.h"

#include <span>


namespace vktg
{


    /// @brief Holds Vulkan buffer and all the information used to (re-)create it.
    struct Buffer {

        vk::Buffer buffer;
        vk::BufferCreateInfo bufferInfo;

        vma::Allocation allocation;
        vma::AllocationCreateInfo allocationCreateInfo;
        vma::AllocationInfo allocationInfo;

        /// @brief Buffer size in bytes.
        /// @return Buffer size in bytes. 
        size_t Size() const { return bufferInfo.size; }
        /// @brief Buffer usage.
        /// @return Buffer usage flags.
        vk::BufferUsageFlags Usage() const { return bufferInfo.usage; }
        /// @brief Buffer memory usage.
        /// @return Vma memory usage.
        vma::MemoryUsage MemoryUsage() const { return allocationCreateInfo.usage; }
        /// @brief Pointer to mapped buffer memory if created with mapped memory.
        /// @return Pointer to mappen buffer data.
        void* Data() const { return allocationInfo.pMappedData; }
    };

    /// @brief Creates Vulkan buffer and stores it in given Buffer object, along with all the data used to create it.
    /// @param buffer Buffer object to store Vulkan buffer and metadata in,
    /// @param bufferSize Size of the buffer to create.
    /// @param bufferUsage Buffer usage flags.
    /// @param memoryUsage Buffer memory usage for vma.
    /// @param flags Buffer allocation flags for vma.
    /// @param sharingMode Queue sharing mode.
    /// @param queueFamilies Queue family indices if concurrent sharing mode is used.
    void CreateBuffer(
        Buffer &buffer,
        size_t bufferSize, vk::BufferUsageFlags bufferUsage, 
        vma::MemoryUsage memoryUsage = vma::MemoryUsage::eGpuOnly, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{},
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive, std::span<uint32_t>  queueFamilies = {}
    );

    /// @brief Resizes given buffer, keeping the same settings used to create it.
    /// @param buffer Buffer object to resize.
    /// @param newSize New size of buffer.
    void ResizeBuffer( Buffer &buffer, size_t newSize);

    /// @brief Destroys allocated buffer.
    /// @param buffer Buffer to sestroy.
    void DestroyBuffer( const Buffer &buffer);

    /// @brief Convenience function for creating staging buffers.
    ///        This will be a CPU side buffer with mapped memory and transfer destination usage.
    /// @param buffer Buffer object to store Vulkan buffer and metadata in,
    /// @param bufferSize Size of the stagingbuffer.
    /// @param data Data to copy into staging buffer after cration.
    void CreateStagingBuffer( Buffer &buffer, size_t bufferSize, const void *data = nullptr);


    /// @brief Stores Vulkan image, full image view and all the inforation used to (re-)create it.
    struct Image {

        vk::Image image;
        vk::ImageView imageView;
        vk::ImageAspectFlags imageAspect;
        vk::ImageCreateInfo imageInfo;

        vma::Allocation allocation;
        vma::AllocationCreateInfo allocationCreateInfo;;
        vma::AllocationInfo allocationInfo;

        /// @brief Image width.
        /// @return Image width.
        uint32_t Width() const { return imageInfo.extent.width; }
        /// @brief Image height.
        /// @return Image height.
        uint32_t Height() const { return imageInfo.extent.height; }
        /// @brief Image format.
        /// @return Vulkan Format.
        vk::Format Format() const { return imageInfo.format; }
        /// @brief Image usage.
        /// @return VUlkan Image usage flags.
        vk::ImageUsageFlags Usage() const { return imageInfo.usage; }
        /// @brief Image memory usage.
        /// @return Vma memory usage.
        vma::MemoryUsage MemoryUsage() const { return allocationCreateInfo.usage; }
        /// @brief Image mip levels.
        /// @return Image mip levels.
        uint32_t MipLevels() const { return imageInfo.mipLevels; }
        /// @brief Number of image layers.
        /// @return Number of image layers.
        uint32_t Layers() const { return imageInfo.arrayLayers; }
        /// @brief Pointer to image data if created with mapped memory.
        /// @return Pointer to mapped image memory.
        void* Data() const { return allocationInfo.pMappedData; }
    };

    /// @brief Creates Vulkan image and stores it in given Image object along all the data used to create it.
    /// @param image Image object to store Vulkan image, default image view and metadata in.
    /// @param width Image width.
    /// @param height Image height.
    /// @param format Image format.
    /// @param usage Image usage.
    /// @param imageAspect Image aspect.
    /// @param mipLevels Image mip levels.
    /// @param layers Image array layers.
    /// @param numSamples Number of image samples.
    /// @param memoryUsage Image memory usage.
    /// @param flags Image creation flags for vma.
    /// @param tiling Image tiling.
    /// @param sharingMode Queue sharing mode.
    /// @param queueFamilies Queu family indices if concurrent sharing mode is used.
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

    /// @brief Resizes Vulkan image and create new image view, keeping the same settings used to create them.
    /// @param image Image object to resize.
    /// @param newWidth New image size.
    /// @param newHeight New image height.
    void ResizeImage( Image &image, uint32_t newWidth, uint32_t newHeight);
    
    /// @brief Destroys given image object
    /// @param image Image object to destroy.
    void DestroyImage( const Image &image);

    /// @brief Creates Vulkan image view for given image object.
    /// @param image Image object holding Vulkan image and image view.
    /// @param format Image format.
    /// @param aspectFlags Image aspect.
    /// @param baseMipLevel Base mip level for image view.
    /// @param mipLevels Number of mip levels to cover.
    /// @param baseLayer Base layer for image view.
    /// @param layerCount Number of array layers to cover.
    /// @param viewType View type.
    /// @return 
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

    /// @brief Destroys given Vulkan image view.
    /// @param imageView Image view to destroy.
    void DestroyImageView( vk::ImageView imageView);
    

} // namespace vktg
