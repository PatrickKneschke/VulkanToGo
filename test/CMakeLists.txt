
add_executable( test_all
    test_main.cpp 
    test_core.cpp 
    test_storage.cpp 
)

target_include_directories( test_all 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" 
)

target_link_libraries( test_all
    PRIVATE vktg
)