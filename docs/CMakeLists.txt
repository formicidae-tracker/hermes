find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

foreach(f ${HERMES_API_DOC_FILES})
	set(DOXYGEN_INPUT_FILES "${DOXYGEN_INPUT_FILES} ${f}")
endforeach(f ${HERMES_API_DOC_FILES})

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/api)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${HERMES_API_DOC_FILES}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                   MAIN_DEPENDENCY ${DOXYFILE_IN}
                   COMMENT "Generate doxygen index")

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
add_custom_target(hermes-docs
                  COMMAND ${SPHINX_BUILD_EXECUTABLE} -b html
                          -Dbreathe_projects.fort-hermes=${DOXYGEN_OUTPUT_DIR}/xml
                          ${SPHINX_SOURCE} ${SPHINX_OUTPUT}
                  DEPENDS ${DOXYGEN_INDEX_FILE}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")

add_dependencies(hermes-docs py_fort_hermes)
