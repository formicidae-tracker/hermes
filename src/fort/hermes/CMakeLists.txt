set(CPP_SRC_FILES Error.cpp
                  NetworkContext.cpp
                  FileContext.cpp)

set(C_SRC_FILES hermes.cpp)


set(HDR_FILES hermes.h
              Error.hpp
              Context.hpp
              NetworkContext.hpp
              FileContext.hpp)


set(PROTO_FILES protobuf/Tag.proto
	            protobuf/FrameReadout.proto
	            protobuf/Header.proto
	            )

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})


include_directories(${PROJECT_BINARY_DIR}/src)


add_library(fort-hermes-cpp SHARED ${CPP_SRC_FILES} ${HDR_FILES} ${PROTO_SRC} ${PROTO_HDR})
set_target_properties(fort-hermes-cpp PROPERTIES VERSION ${PROJECT_VERSION_API}
                                                 SOVERSION ${PROJECT_VERSION_ABI})

target_link_libraries(fort-hermes-cpp ${PROTOBUF_LIBRARIES})

install(TARGETS fort-hermes-cpp DESTINATION lib)

install(FILES ${HDR_FILES} ${PROTO_HDR} ${PROTO_FILES} DESTINATION ${INCLUDE_INSTALL_DIR})


add_library(fort-hermes SHARED ${C_SRC_FILES} ${HDR_FILES})
set_target_properties(fort-hermes PROPERTIES VERSION ${PROJECT_VERSION_API}
                                             SOVERSION ${PROJECT_VERSION_ABI})

target_link_libraries(fort-hermes fort-hermes-cpp ${Boost_LIBRARIES})


install(TARGETS fort-hermes DESTINATION lib)


set(UTEST_HDR_FILES UTestDataUTest.hpp)
set(UTEST_SRC_FILES UTestDataUTest.cpp
                    main-check.cpp
                    ErrorUTest.cpp
                    FileContextUTest.cpp
                    NetworkContextUTest.cpp
                    )

add_executable(fort-hermes-tests ${UTEST_HDR_FILES} ${UTEST_SRC_FILES})
target_link_libraries(fort-hermes-tests fort-hermes gtest)

add_test(NAME fort-hermes-tests COMMAND fort-hermes-tests)
add_dependencies(check fort-hermes-tests)
