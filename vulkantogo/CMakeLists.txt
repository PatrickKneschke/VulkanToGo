

#vma
add_library( vma INTERFACE)
target_include_directories( vma INTERFACE vma)


add_library( vktg)
target_sources( vktg PRIVATE
    vulkantogo.h 
    vk_core.h 

    vk_core.cpp 
)

target_link_libraries( vktg
    PUBLIC vulkan 
    PUBLIC glfw 
    PUBLIC vma 
)

set_target_properties( vktg
    PROPERTIES CXX_STANDARD 20 
)