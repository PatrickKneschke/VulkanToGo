
# Function for building single example
function( buildExample EXAMPLE_NAME)
	add_executable( ${EXAMPLE_NAME} 
		"${EXAMPLE_NAME}.cpp"
	)
	target_include_directories( ${EXAMPLE_NAME}
		PRIVATE "${PROJECT_SOURCE_DIR}/vulkantogo"
	)
	target_link_libraries( ${EXAMPLE_NAME} 
		PRIVATE vktg 
	)
endfunction(buildExample)


set( EXAMPLES
	compute_image 
	triangle 
)

foreach( EXAMPLE ${EXAMPLES})
	buildExample( ${EXAMPLE})
endforeach( EXAMPLE)