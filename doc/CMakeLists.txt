# CMake configuration for the doc directory

find_package(Doxygen REQUIRED)

# Set input and output files
set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Request to configure the file
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

# Add a custom target to generate the documentation
add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)