
#include "storage.h"


namespace vktg
{


    Buffer CreateBuffer( size_t bufferSize, vk::BufferUsageFlags bufferUsage, vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags) {

        Buffer newBuffer{
            .bufferSize = bufferSize,
            .bufferUsage = bufferUsage,
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


} // namespace vktg
