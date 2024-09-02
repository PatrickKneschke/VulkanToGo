#pragma once


#include "vk_core.h"


namespace vktg
{


    struct Buffer {
        vk::Buffer buffer;
        vma::Allocation allocation;
        vma::AllocationInfo allocationInfo;
        
        const size_t bufferSize = 0;
        const vk::BufferUsageFlags bufferUsage;
        const vma::MemoryUsage memoryUsage = vma::MemoryUsage::eUnknown;
    };


    Buffer CreateBuffer( size_t bufferSize, vk::BufferUsageFlags bufferUsage, vma::MemoryUsage memoryUsage, vma::AllocationCreateFlags flags = vma::AllocationCreateFlags{});
    void DestroyBuffer( const Buffer &buffer);
    

} // namespace vktg
