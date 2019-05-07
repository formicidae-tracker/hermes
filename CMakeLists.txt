cmake_policy(SET CMP0048 NEW)

project(hermes VERSION 0.1.0
               LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2)

set(VERSION_API ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
if(${PROJECT_VERSION_MAJOR} EQUAL 0)
	set(VERSION_ABI ${VERSION_API})
else(${PROJECT_VERSION_MAJOR} EQUAL 0)
	set(VERSION_ABI ${PROJECT_VERSION_MAJOR})
endif(${PROJECT_VERSION_MAJOR} EQUAL 0)


find_package(Protobuf 3.5.0 REQUIRED)


set(INCLUDE_DIR include/fort/hermes)

add_subdirectory(protobuf)

add_subdirectory(src)
