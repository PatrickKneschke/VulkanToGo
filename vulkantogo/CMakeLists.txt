

#vma
add_library( vma INTERFACE)
target_include_directories( vma INTERFACE vma)


add_library( vktg STATIC)
target_sources( vktg PRIVATE
    vulkantogo.h 
    vk_core.h 
    storage.h 
    swapchain.h 
    synchronization.h 
    commands.h 
    pipelines.h 
    descriptors.h 
    samplers.h 
    rendering.h 
    transfer.h 
    submit_context.h 
    
    util/deletion_stack.h 
    util/timer.h
    util/frame_handler.h
    util/input_handler.h

    vk_core.cpp 
    storage.cpp 
    swapchain.cpp 
    synchronization.cpp 
    commands.cpp 
    pipelines.cpp 
    descriptors.cpp
    samplers.cpp
    rendering.cpp 
    transfer.cpp 
    submit_context.cpp 

    util/timer.cpp 
    util/frame_handler.cpp 
    util/input_handler.cpp 
)

target_link_libraries( vktg
    PUBLIC vulkan 
    PUBLIC glfw 
    PUBLIC vma 
)