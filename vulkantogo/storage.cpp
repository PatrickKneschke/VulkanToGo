
#include "storage.h"


namespace vktg
{


    void CreateBuffer( 
        Buffer &buffer, size_t bufferSize, vk::BufferUsageFlags bufferUsage, 
        vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags, 
        vk::SharingMode sharingMode, std::span<uint32_t>  queueFamilies)
    {
        buffer.bufferUsage = bufferUsage;
        buffer.bufferSize = bufferSize;
        buffer.memoryUsage = memoryUsage;

        auto bufferInfo = vk::BufferCreateInfo{}
            .setSize( buffer.bufferSize )
            .setUsage( buffer.bufferUsage )
            .setSharingMode( sharingMode )
            .setQueueFamilyIndexCount( queueFamilies.size() )
            .setPQueueFamilyIndices( queueFamilies.data() );
        
        auto allocInfo = vma::AllocationCreateInfo{}
            .setUsage( buffer.memoryUsage )
            .setFlags( flags );

        VK_CHECK( Allocator().createBuffer( &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, &buffer.allocationInfo) );
    }


    void CreateStagingBuffer(Buffer buffer, size_t bufferSize) {

        CreateBuffer(
            buffer,
            bufferSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vma::MemoryUsage::eCpuOnly,
            vma::AllocationCreateFlagBits::eMapped
        );
    }


    void DestroyBuffer(const Buffer &buffer) {

        if (!buffer.buffer)
        {
            return;
        }

        Allocator().destroyBuffer( buffer.buffer, buffer.allocation);
    }


    void CreateImage(
        Image &image,
        uint32_t width, uint32_t height, 
        vk::Format format, vk::ImageUsageFlags usage, 
        vk::ImageAspectFlags imageAspect, 
        uint32_t mipLevels, uint32_t layers, vk::SampleCountFlagBits numSamples, 
        vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags, 
        vk::ImageTiling tiling, 
        vk::SharingMode sharingMode, std::span<uint32_t>  queueFamilies)
    {
        image.imageAspect = imageAspect;
        image.imageInfo = vk::ImageCreateInfo{}
            .setImageType( vk::ImageType::e2D )
            .setExtent( vk::Extent3D{ width, height, 1} )
            .setFormat( format )
            .setUsage( usage )
            .setMipLevels( mipLevels )
            .setArrayLayers( layers )
            .setTiling( tiling )
            .setSamples( numSamples )
            .setSharingMode( sharingMode )
            .setQueueFamilyIndexCount( queueFamilies.size() )
            .setPQueueFamilyIndices( queueFamilies.data() )
            .setInitialLayout( vk::ImageLayout::eUndefined );
        image.memoryUsage = memoryUsage;

        auto allocInfo = vma::AllocationCreateInfo{}
            .setUsage( image.memoryUsage )
            .setFlags( flags );
    
        VK_CHECK( Allocator().createImage( &image.imageInfo, &allocInfo, &image.image, &image.allocation, &image.allocationInfo) );
        image.imageView = CreateImageView( 
            image.image, image.imageInfo.format, image.imageAspect, 
            0, image.imageInfo.mipLevels, 0, image.imageInfo.arrayLayers
        );
    }


    void DestroyImage(const Image &image) {

        DestroyImageView( image.imageView);

        if (!image.image)
        {
            return;
        }
        Allocator().destroyImage( image.image, image.allocation);
    }


    vk::ImageView CreateImageView(
        vk::Image image, 
        vk::Format format, vk::ImageAspectFlags aspectFlags, 
        uint32_t baseMipLevel, uint32_t mipLevels, uint32_t baseLayer, uint32_t layerCount, 
        vk::ImageViewType viewType)
    {
        auto imageViewInfo = vk::ImageViewCreateInfo{}
            .setImage( image )
            .setViewType( viewType )
            .setFormat( format )
            .setSubresourceRange( vk::ImageSubresourceRange{aspectFlags, baseMipLevel, mipLevels, baseLayer, layerCount} );
                
        vk::ImageView imageView;
        VK_CHECK( Device().createImageView( &imageViewInfo, nullptr, &imageView) );
            
        return imageView;
    }


    void DestroyImageView(vk::ImageView imageView) {

        if (!imageView)
        {
            return;
        }

        Device().destroyImageView( imageView);
    }


} // namespace vktg
