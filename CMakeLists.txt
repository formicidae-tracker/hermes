cmake_policy(SET CMP0048 NEW)

project(hermes VERSION 0.1.8
               LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2)

set(VERSION_API ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
if(${PROJECT_VERSION_MAJOR} EQUAL 0)
	set(VERSION_ABI ${VERSION_API})
else(${PROJECT_VERSION_MAJOR} EQUAL 0)
	set(VERSION_ABI ${PROJECT_VERSION_MAJOR})
endif(${PROJECT_VERSION_MAJOR} EQUAL 0)

set(CMAKE_CXX_STANDARD 11)


find_package(Protobuf 3.3.0 REQUIRED)

include(ExternalProject)
ExternalProject_Add(ASIO
	URL https://sourceforge.net/projects/asio/files/asio/1.10.8%20%28Stable%29/asio-1.10.8.tar.bz2/download
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ./configure --prefix=${PROJECT_BINARY_DIR}/ASIO-prefix
	BUILD_COMMAND echo "done"
	TEST_BEFORE_INSTALL 1
	TEST_COMMAND make check -j 7
	INSTALL_COMMAND make install)

set(INCLUDE_INSTALL_DIR include/fort-hermes)
set(LIB_INSTALL_DIR lib )


include(CMakePackageConfigHelpers)
configure_package_config_file(FortHermesCppConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfig.cmake
	INSTALL_DESTINATION ${LIB_INSTALL_DIR}/FortHermesCpp/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/FortHermesCpp/cmake )

include_directories(${PROJECT_BINARY_DIR}/ASIO-prefix/include
	                ${Boost_INCLUDE_DIRS})


add_subdirectory(protobuf)

add_subdirectory(src)

add_subdirectory(pkgconfig)
