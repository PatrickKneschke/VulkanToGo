
add_executable( test_all
    test_main.cpp 
)

target_link_libraries( test_all
    PRIVATE vktg
)