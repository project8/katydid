# Based on code by Stefan Majewsky, found at http://majewsky.wordpress.com/2010/08/14/tip-of-the-day-cmake-and-doxygen/ on 12/15/2011.
# To change where the Doxygen site is built, modify the WORKING_DIRECTORY (line 10)

# add a target to generate API documentation with Doxygen
find_package (Doxygen REQUIRED)
if (DOXYGEN_FOUND)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (${PROJECT_NAME}_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)


###########
#### sphinx
find_package(Sphinx REQUIRED)
execute_process( COMMAND python -c "import breathe"
                 RESULT_VARIABLE BREATH_FOUND_RESULT
                 OUTPUT_QUIET
                 ERROR_QUIET
               )
if(NOT BREATH_FOUND_RESULT EQUAL 0)
    message( ERROR "the python breathe package is required for sphinx+doxygen" )
endif(NOT BREATH_FOUND_RESULT EQUAL 0)

if(NOT DEFINED SPHINX_THEME)
    execute_process( COMMAND python -c "import sphinx_rtd_theme"
                     RESULT_VARIABLE SPHINX_RTD_THEME_FOUND
                     OUTPUT_QUIET
                     ERROR_QUIET
                   )
    if(SPHINX_RTD_THEME_FOUND EQUAL 0)
        set(SPHINX_THEME sphinx_rtd_theme CACHE STRING "Sphinx string name")
    else(SPHINX_RTD_THEME_FOUND EQUAL 0)
        message( WARNING "sphinx_rtd_theme python package not found, using default sphinx theme")
        set(SPHINX_THEME classic CACHE STRING "Sphinx string name")
    endif(SPHINX_RTD_THEME_FOUND EQUAL 0)
endif()

if(NOT DEFINED SPHINX_THEME_DIR)
    set(SPHINX_THEME_DIR)
endif()

message( STATUS "Katydid_VERSION is: ${${PROJECT_NAME}_VERSION}" )

# configured documentation tools and intermediate build results
set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")
# Sphinx cache with pickled ReST documents
set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")
# html output directory
set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
    "${BINARY_BUILD_DIR}/conf.py.in.tmp"
    @ONLY
)
configure_file(
    "${BINARY_BUILD_DIR}/conf.py.in.tmp"
    "${BINARY_BUILD_DIR}/conf.py"
    @ONLY
)
file (REMOVE ${BINARY_BUILD_DIR}/conf.py.in.tmp)

add_custom_target(
    ${PROJECT_NAME}_sphinx ALL
    ${SPHINX_EXECUTABLE}
        -q -b html
        -c "${BINARY_BUILD_DIR}"
        -d "${SPHINX_CACHE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${SPHINX_HTML_DIR}"
    DEPENDS ${PROJECT_NAME}_doc
    COMMENT "Building HTML documentation with Sphinx"
)
