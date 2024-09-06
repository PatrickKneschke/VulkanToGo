
#include "synchronization.h"


namespace vktg
{


    vk::Fence CreateFence( vk::FenceCreateFlagBits flags) {

        auto fenceInfo = vk::FenceCreateInfo{}
            .setFlags( flags);

        vk::Fence fence;
        VK_CHECK( Device().createFence( &fenceInfo, nullptr, &fence) );

        return fence;
    }


    void DestroyFence( vk::Fence fence) {
    
        Device().destroyFence( fence);
    }

    
    vk::Semaphore CreateSemaphore( vk::SemaphoreType type) {

        auto semaphoreTypeInfo = vk::SemaphoreTypeCreateInfo{}
            .setSemaphoreType( type );

        auto semaphoreInfo = vk::SemaphoreCreateInfo{}
            .setPNext( &semaphoreTypeInfo );

        vk::Semaphore semaphore;    
        VK_CHECK( Device().createSemaphore( &semaphoreInfo, nullptr, &semaphore) );

        return semaphore;
    }


    void DestroySemaphore(vk::Semaphore semaphore) {
    
        Device().destroySemaphore( semaphore);
    }


} // namespace vktg