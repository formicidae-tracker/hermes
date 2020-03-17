include_directories(${PROJECT_SOURCE_DIR}/src)


add_executable(hermes-file-reader file-reader.c)
target_link_libraries(hermes-file-reader fort-hermes)


add_executable(hermes-network-reader network-reader.c)
target_link_libraries(hermes-network-reader fort-hermes)
