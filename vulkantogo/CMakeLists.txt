

#vma
add_library( vma INTERFACE)
target_include_directories( vma INTERFACE vma)


add_library( vktg STATIC)
target_sources( vktg PRIVATE
    vulkantogo.h 
    vk_core.h 
    deletion_stack.h 
    storage.h 
    swapchain.h 
    synchronization.h 
    commands.h 
    pipelines.h 
    descriptors.h 
    samplers.h 

    vk_core.cpp 
    storage.cpp 
    swapchain.cpp 
    synchronization.cpp 
    commands.cpp 
    pipelines.cpp 
    descriptors.cpp
    samplers.cpp
)

target_link_libraries( vktg
    PUBLIC vulkan 
    PUBLIC glfw 
    PUBLIC vma 
)