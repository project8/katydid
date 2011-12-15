# Based on code by Stefan Majewsky, found at http://majewsky.wordpress.com/2010/08/14/tip-of-the-day-cmake-and-doxygen/ on 12/15/2011.
# Builds in the source tree (to instead build in the binary tree, switch CMAKE_CURRENT_SOURCE_DIR to CMAKE_CURRENT_BINARY_DIR in the line that starts with WORKING_DIRECTORY.

# add a target to generate API documentation with Doxygen
find_package (Doxygen)
if (DOXYGEN_FOUND)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)