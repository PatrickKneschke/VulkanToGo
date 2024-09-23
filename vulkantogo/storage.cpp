
#include "storage.h"


namespace vktg
{


    Buffer CreateBuffer( size_t bufferSize, vk::BufferUsageFlags bufferUsage, vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags) {

        Buffer newBuffer{
            .bufferUsage = bufferUsage,
            .bufferSize = bufferSize,
            .memoryUsage = memoryUsage
        };

        auto bufferInfo = vk::BufferCreateInfo{}
            .setSize( newBuffer.bufferSize )
            .setUsage( newBuffer.bufferUsage );
        
        auto allocInfo = vma::AllocationCreateInfo{}
            .setUsage( newBuffer.memoryUsage )
            .setFlags( flags );

        VK_CHECK( Allocator().createBuffer( &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.allocationInfo) );

        return newBuffer;
    }


    void DestroyBuffer(const Buffer &buffer) {

        if (!buffer.buffer)
        {
            return;
        }

        Allocator().destroyBuffer( buffer.buffer, buffer.allocation);
    }


    Image CreateImage(
        uint32_t width, uint32_t height, 
        vk::Format format, vk::ImageUsageFlags usage, 
        vk::ImageAspectFlags imageAspect, 
        uint32_t mipLevels, uint32_t layers, vk::SampleCountFlagBits numSamples, 
        vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags, 
        vk::ImageTiling tiling, 
        vk::SharingMode sharingMode, uint32_t queueFamilyCount, const uint32_t *pQueueFamilies)
    {
        Image newImage{
            .imageAspect = imageAspect,
            .imageInfo = vk::ImageCreateInfo{}
                .setImageType( vk::ImageType::e2D )
                .setExtent( vk::Extent3D{ width, height, 1} )
                .setFormat( format )
                .setUsage( usage )
                .setMipLevels( mipLevels )
                .setArrayLayers( layers )
                .setTiling( tiling )
                .setSamples( numSamples )
                .setSharingMode( sharingMode )
                .setQueueFamilyIndexCount( queueFamilyCount )
                .setPQueueFamilyIndices( pQueueFamilies )
                .setInitialLayout( vk::ImageLayout::eUndefined ),
            .memoryUsage = memoryUsage,
        };

        auto allocInfo = vma::AllocationCreateInfo{}
            .setUsage( newImage.memoryUsage )
            .setFlags( flags );
    
        VK_CHECK( Allocator().createImage( &newImage.imageInfo, &allocInfo, &newImage.image, &newImage.allocation, &newImage.allocationInfo) );
        newImage.imageView = CreateImageView( 
            newImage.image, newImage.imageInfo.format, newImage.imageAspect, 
            0, newImage.imageInfo.mipLevels, 0, newImage.imageInfo.arrayLayers
        );

        return newImage;
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
