#pragma once


#include "vk_core.h"


namespace vktg
{


    vk::Fence CreateFence( vk::FenceCreateFlags flags = vk::FenceCreateFlagBits::eSignaled);
    
    void DestroyFence( vk::Fence fence);


    vk::Semaphore CreateSemaphore( vk::SemaphoreType type = vk::SemaphoreType::eBinary);
    
    void DestroySemaphore( vk::Semaphore semaphore);


    

    
} // namespace vktg
