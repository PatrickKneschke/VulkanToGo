
# Function for building single example
function( buildExample EXAMPLE_NAME)
	add_executable( ${EXAMPLE_NAME} 
		"${EXAMPLE_NAME}.cpp"
		shared/utility.h 
		shared/utility.cpp 
	)
	target_include_directories( ${EXAMPLE_NAME}
		PRIVATE "${PROJECT_SOURCE_DIR}/vulkantogo"
		PRIVATE "${PROJECT_SOURCE_DIR}/examples/shared"
	)
	target_link_libraries( ${EXAMPLE_NAME} 
		PRIVATE vktg 
	)

endfunction(buildExample)


set( EXAMPLES
	compute_image 
	triangle 
	textured_mesh
)

foreach( EXAMPLE ${EXAMPLES})
	buildExample( ${EXAMPLE})
endforeach( EXAMPLE)