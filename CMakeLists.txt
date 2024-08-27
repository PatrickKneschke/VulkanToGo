
cmake_minimum_required( VERSION 3.8)

project( "vulkantogo")

add_subdirectory( vulkantogo)
add_subdirectory( test)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")