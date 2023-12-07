# libfort-hermes - Tracking File I/O library.
#
# Copyright (C) 2018-2023 Universitée de Lausanne
#
# This file is part of libfort-hermes.
#
# libfort-hermes is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# libfort-hermes is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# libfort-hermes.  If not, see <http://www.gnu.org/licenses/>.

cmake_policy(SET CMP0048 NEW)

cmake_minimum_required(VERSION 3.11)

project(hermes LANGUAGES C CXX)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(FORT_HERMES_MAIN 1)
endif(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)

option(ENABLE_COVERAGE "Enables code coverage" Off)
option(BUILD_PYTHON "Enables python building" Off)
option(BUILD_DOCS "Enables building of documentation" Off)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(VersionFromGit)
version_from_git()

find_package(Protobuf 3.3.0 REQUIRED)
find_package(Asio 1.12 REQUIRED)

include(FetchContent)

set(CPPTRACE_BUILD_SHARED On)
include(FetchContent)
FetchContent_Declare(
	cpptrace
	GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
	GIT_TAG v0.3.0
)
FetchContent_MakeAvailable(cpptrace)

if(FORT_HERMES_MAIN)
	set(INSTALL_GTEST Off)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG 76bb2afb8b522d24496ad1c757a49784fbfa2e42
	)
	# For Windows: Prevent overriding the parent project's compiler/linker
	# settings
	set(gtest_force_shared_crt
		ON
		CACHE BOOL "" FORCE
	)
	FetchContent_MakeAvailable(googletest)

	include(GoogleTest)
	enable_testing()
	set(MAKE_CHECK_TEST_COMMAND ${CMAKE_CTEST_COMMAND} -V)
	add_custom_target(check COMMAND ${MAKE_CHECK_TEST_COMMAND})
endif(FORT_HERMES_MAIN)

set(INCLUDE_INSTALL_DIR include/fort/hermes)
set(LIB_INSTALL_DIR lib)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	FortHermesCppConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfig.cmake
	INSTALL_DESTINATION ${LIB_INSTALL_DIR}/FortHermesCpp/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfigVersion.cmake
	VERSION "${PROJECT_VERSION}"
	COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfig.cmake
			  ${CMAKE_CURRENT_BINARY_DIR}/FortHermesCppConfigVersion.cmake
		DESTINATION ${LIB_INSTALL_DIR}/FortHermesCpp/cmake
)

if(FORT_HERMES_MAIN AND ENABLE_COVERAGE)
	include(CodeCoverage)
	enable_coverage()
	setup_target_for_coverage(
		NAME
		coverage
		DEPENDENCIES
		check
		LCOV_OPTIONS
		--exclude
		"'*UTest.*'"
		--exclude
		"'*/main-check.cpp'"
		--include
		"'${PROJECT_SOURCE_DIR}/src/fort/hermes/*'"
	)
endif(FORT_HERMES_MAIN AND ENABLE_COVERAGE)

add_subdirectory(src/fort/hermes)

add_subdirectory(pkgconfig)

add_subdirectory(examples)

if(FORT_HERMES_MAIN AND (BUILD_PYTHON OR BUILD_DOCS))
	add_subdirectory(src/python)
endif(FORT_HERMES_MAIN AND (BUILD_PYTHON OR BUILD_DOCS))

if(FORT_HERMES_MAIN AND BUILD_DOCS)
	add_subdirectory(docs)
endif(FORT_HERMES_MAIN AND BUILD_DOCS)
