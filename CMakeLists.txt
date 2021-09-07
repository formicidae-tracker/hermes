cmake_policy(SET CMP0048 NEW)

project(hermes LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(VersionFromGit)
version_from_git()


find_package(Protobuf 3.3.0 REQUIRED)
find_package(Asio 1.12 REQUIRED)

include(FetchGoogleTest)

fetch_google_test(VERSION release-1.10.0)

include(GoogleTest)
enable_testing()
set(MAKE_CHECK_TEST_COMMAND ${CMAKE_CTEST_COMMAND} -V)
if(NOT TARGET check)
	add_custom_target(check COMMAND ${MAKE_CHECK_TEST_COMMAND})
endif(NOT TARGET check)

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

option(FORT_HERMES_ENABLE_COVERAGE "Enables code coverage" Off)
option(FORT_HERMES_BUILD_PYTHON "Enables python building" Off)
option(FORT_HERMES_BUILD_DOCS "Enables building of documentation" Off)

if(FORT_HERMES_ENABLE_COVERAGE)
	include(CodeCoverage)
	enable_coverage()
	setup_target_for_coverage(NAME coverage
	                          DEPENDENCIES check
	                          LCOV_OPTIONS --exclude "'*UTest.*'"
	                                       --exclude "'*/main-check.cpp'"
	                                       --include "'${PROJECT_SOURCE_DIR}/src/fort/hermes/*'"
	                          )
endif(FORT_HERMES_ENABLE_COVERAGE)

add_subdirectory(src/fort/hermes)

add_subdirectory(pkgconfig)

add_subdirectory(examples)

if(FORT_HERMES_BUILD_PYTHON)
	add_subdirectory(src/python)
endif(FORT_HERMES_BUILD_PYTHON)

if(FORT_HERMES_BUILD_DOCS)
	add_subdirectory(docs)
endif(FORT_HERMES_BUILD_DOCS)
