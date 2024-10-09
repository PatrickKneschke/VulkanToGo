
add_executable( test_all
    test_main.cpp 
    test_core.cpp 
    test_storage.cpp 
    test_swapchain.cpp 
    test_synchronization.cpp 
    test_commands.cpp 
    test_pipelines.cpp 
    test_descriptors.cpp 
    test_samplers.cpp 
    test_rendering.cpp 
    test_transfer.cpp 
    test_submit_context.cpp 
    test_timer.cpp 
)

target_include_directories( test_all 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" 
)

target_link_libraries( test_all
    PRIVATE vktg
)