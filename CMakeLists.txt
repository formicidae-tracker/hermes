cmake_policy(SET CMP0048 NEW)

project(hermes LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2)


set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(VersionFromGit)
version_from_git()


find_package(Protobuf 3.3.0 REQUIRED)
find_package(Asio 1.12 REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})

set(INCLUDE_INSTALL_DIR include/fort/hermes)
set(LIB_INSTALL_DIR lib )


include(CMakePackageConfigHelpers)
configure_package_config_file(FortHermesCppConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfig.cmake
	INSTALL_DESTINATION ${LIB_INSTALL_DIR}/FortHermesCpp/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfigVersion.cmake
	VERSION "${PROJECT_VERSION}"
	COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/FortHermesCpp/cmake )

add_subdirectory(src)

add_subdirectory(pkgconfig)

add_subdirectory(examples)

add_subdirectory(docs)
