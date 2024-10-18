
#include "storage.h"


namespace vktg
{


    void CreateBuffer( 
        Buffer &buffer, size_t bufferSize, vk::BufferUsageFlags bufferUsage, 
        vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags, 
        vk::SharingMode sharingMode, std::span<uint32_t>  queueFamilies)
    {
        buffer.bufferInfo = vk::BufferCreateInfo{}
            .setSize( bufferSize )
            .setUsage( bufferUsage )
            .setSharingMode( sharingMode )
            .setQueueFamilyIndexCount( queueFamilies.size() )
            .setPQueueFamilyIndices( queueFamilies.data() );
        
        buffer.allocationCreateInfo = vma::AllocationCreateInfo{}
            .setUsage( memoryUsage )
            .setFlags( flags );

        VK_CHECK( Allocator().createBuffer( &buffer.bufferInfo, &buffer.allocationCreateInfo, &buffer.buffer, &buffer.allocation, &buffer.allocationInfo) );
    }


    void ResizeBuffer(Buffer &buffer, size_t newSize) {

        DestroyBuffer( buffer);
        buffer.bufferInfo.setSize( newSize );
        VK_CHECK( Allocator().createBuffer( &buffer.bufferInfo, &buffer.allocationCreateInfo, &buffer.buffer, &buffer.allocation, &buffer.allocationInfo) );
    }


    void DestroyBuffer(const Buffer &buffer) {

        if (!buffer.buffer)
        {
            return;
        }

        Allocator().destroyBuffer( buffer.buffer, buffer.allocation);
    }


    void CreateStagingBuffer( Buffer &buffer, size_t bufferSize, void *data) {

        CreateBuffer(
            buffer,
            bufferSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vma::MemoryUsage::eCpuOnly,
            vma::AllocationCreateFlagBits::eMapped
        );

        // fill staging buffer with provided data
        if (data != nullptr)
        {
            memcpy( buffer.Data(), data, bufferSize);
        }
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

        image.allocationCreateInfo = vma::AllocationCreateInfo{}
            .setUsage( memoryUsage )
            .setFlags( flags );
    
        VK_CHECK( Allocator().createImage( &image.imageInfo, &image.allocationCreateInfo, &image.image, &image.allocation, &image.allocationInfo) );
        image.imageView = CreateImageView( 
            image.image, image.imageInfo.format, image.imageAspect, 
            0, image.imageInfo.mipLevels, 0, image.imageInfo.arrayLayers
        );
    }

    
    void ResizeImage(Image &image, uint32_t newWidth, uint32_t newHeight) {

        DestroyImage( image);
        image.imageInfo.setExtent( vk::Extent3D{newWidth, newHeight, 1} );
        VK_CHECK( Allocator().createImage( &image.imageInfo, &image.allocationCreateInfo, &image.image, &image.allocation, &image.allocationInfo) );
        image.imageView = CreateImageView( 
            image.image, image.Format(), image.imageAspect, 
            0, image.MipLevels(), 0, image.Layers()
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
