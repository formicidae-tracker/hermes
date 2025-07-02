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

set(CPP_SRC_FILES
	Error.cpp #
	NetworkContext.cpp #
	FileContext.cpp #
	CheckHeader.hpp #
	CheckHeader.cpp
	details/FileSequence.hpp
	details/FileSequence.cpp
)

set(C_SRC_FILES hermes.cpp)

set(HDR_FILES
	hermes.h #
	Error.hpp #
	Context.hpp #
	NetworkContext.hpp #
	FileContext.hpp
)

set(PROTO_FILES
	protobuf/Tag.proto #
	protobuf/FrameReadout.proto #
	protobuf/Header.proto
)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})

include_directories(${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR}/src)

add_library(
	fort-hermes-cpp SHARED ${CPP_SRC_FILES} ${HDR_FILES} ${PROTO_SRC}
						   ${PROTO_HDR}
)
set_target_properties(
	fort-hermes-cpp PROPERTIES VERSION ${PROJECT_VERSION_API}
							   SOVERSION ${PROJECT_VERSION_ABI}
)

target_link_libraries(
	fort-hermes-cpp protobuf::libprotobuf Asio::libasio cpptrace::cpptrace
)

install(TARGETS fort-hermes-cpp DESTINATION lib)

install(FILES ${HDR_FILES} ${PROTO_HDR} ${PROTO_FILES}
		DESTINATION ${INCLUDE_INSTALL_DIR}
)

add_library(fort-hermes SHARED ${C_SRC_FILES} ${HDR_FILES})
set_target_properties(
	fort-hermes PROPERTIES VERSION ${PROJECT_VERSION_API}
						   SOVERSION ${PROJECT_VERSION_ABI}
)

target_link_libraries(fort-hermes fort-hermes-cpp ${Boost_LIBRARIES})

install(TARGETS fort-hermes DESTINATION lib)

set(UTEST_HDR_FILES UTestDataUTest.hpp)
set(UTEST_SRC_FILES
	UTestDataUTest.cpp
	main-check.cpp
	ErrorUTest.cpp
	FileContextUTest.cpp
	NetworkContextUTest.cpp
	StreamServerUTest.hpp
	StreamServerUTest.cpp
	CheckHeaderUTest.cpp
	details/FileSequenceUTest.cpp
)
if(FORT_HERMES_MAIN)
	add_executable(fort-hermes-tests ${UTEST_HDR_FILES} ${UTEST_SRC_FILES})
	target_link_libraries(
		fort-hermes-tests fort-hermes GTest::gtest_main GTest::gmock_main
	)

	gtest_discover_tests(fort-hermes-tests)
	add_dependencies(check fort-hermes-tests)
else(FORT_HERMES_MAIN)
	add_library(fort-hermes::libfort-hermes INTERFACE IMPORTED GLOBAL)
	target_link_libraries(fort-hermes::libfort-hermes INTERFACE fort-hermes-cpp)
	target_include_directories(
		fort-hermes::libfort-hermes INTERFACE ${PROJECT_SOURCE_DIR}/src
											  ${PROJECT_BINARY_DIR}/src
	)
endif(FORT_HERMES_MAIN)

foreach(h ${HDR_FILES})
	list(APPEND HERMES_API_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h})
endforeach(h ${HDR_FILES})
set(HERMES_API_DOC_FILES
	${HERMES_API_DOC_FILES}
	PARENT_SCOPE
)
