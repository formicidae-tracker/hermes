find_package(Python REQUIRED)

add_custom_target(py_fort_myrmidon ALL
                  COMMAND ${Python_EXECUTABLE} setup.py build -b ${CMAKE_CURRENT_BINARY_DIR}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(py_fort_myrmidon-tests
                  COMMAND ${Python_EXECUTABLE} runTests.py
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(check py_fort_myrmidon-tests)
