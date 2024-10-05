
#pragma once


#include "vk_core.h"


namespace vktg
{


    struct SubmitContext {
        vk::Queue queue;
        vk::CommandPool cmdPool;
        vk::CommandBuffer cmd;
        vk::Fence fence;

        void Begin();
        void End();
        void Submit();
    };

    SubmitContext CreateSubmitContext( QueueType queueType);
    
    void DestroySubmitContext( const SubmitContext &context);

    
} // namespace vktg
