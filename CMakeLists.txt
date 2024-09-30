
cmake_minimum_required( VERSION 3.12)

project( "vulkantogo")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set( CMAKE_CXX_STANDARD 20)

add_subdirectory( vulkantogo)
add_subdirectory( test)
add_subdirectory( examples)
