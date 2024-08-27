
cmake_minimum_required( VERSION 3.8)

project( "vulkantogo")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


add_subdirectory( vulkantogo)
add_subdirectory( test)

add_executable( ${PROJECT_NAME}
    main.cpp 
)

target_include_directories( ${PROJECT_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vulkantogo"
)

target_link_libraries( ${PROJECT_NAME} 
    PRIVATE vktg
)

set_property( TARGET ${PROJECT_NAME} 
    PROPERTY CXX_STANDARD 20
)