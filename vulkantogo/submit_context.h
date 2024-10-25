
#pragma once


#include "vk_core.h"


namespace vktg
{


    /// @brief Used for one time submit of commands parallel to the main render loop.
    struct SubmitContext {
        vk::Queue queue;
        vk::CommandPool cmdPool;
        vk::CommandBuffer cmd;
        vk::Fence fence;

        /// @brief Resets wait fence, command buffer and begins new command recording.
        void Begin();
        /// @brief Ends command recording.
        void End();
        /// @brief Submits recorded commands to associated queue.
        void Submit();
    };

    /// @brief Creates submit context for given queue type.
    /// @param queueType Queue type this submit sontext records commands for.
    /// @return Submit context.
    SubmitContext CreateSubmitContext( QueueType queueType);
    
    /// @brief Destroys given submit context.
    /// @param context Submit context to destroy.
    void DestroySubmitContext( const SubmitContext &context);

    
} // namespace vktg
