

add_subdirectory( ext)


# compile options
set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-Wformat;-Wshadow;-Wconversion;-pthread;-std=c++20")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")

add_compile_options(
    "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>"
)

add_library( vktg)
target_sources( vktg PRIVATE
    vulkantogo.h 
)

# target_link_libraries( vktg

# )

set_target_properties( vktg
    PROPERTIES CXX_STANDARD 20 
)