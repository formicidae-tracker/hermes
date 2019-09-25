


set(CPP_SRC_FILES Path.cpp
                  Error.cpp
                  NetworkContext.cpp
                  FileContext.cpp)

set(C_SRC_FILES hermes.cpp)


set(HDR_FILES hermes.h
              Path.h
              Error.h
              Context.h
              NetworkContext.h
              FileContext.h)

include_directories(${PROJECT_BINARY_DIR}/protobuf)


add_library(fort-hermes-cpp SHARED ${CPP_SRC_FILES} ${HDR_FILES} ${PROTO_SRC} ${PROTO_HDR})
set_target_properties(fort-hermes-cpp PROPERTIES VERSION ${VERSION_API}
                                      SOVERSION ${VERSION_ABI})

target_link_libraries(fort-hermes-cpp ${PROTOBUF_LIBRARIES})

install(TARGETS fort-hermes-cpp DESTINATION lib)

install(FILES ${HDR_FILES} DESTINATION ${INCLUDE_INSTALL_DIR})


add_library(fort-hermes SHARED ${C_SRC_FILES} ${HDR_FILES})
set_target_properties(fort-hermes PROPERTIES VERSION ${VERSION_API}
                                                     SOVERSION ${VERSION_ABI})

target_link_libraries(fort-hermes fort-hermes-cpp ${Boost_LIBRARIES})


install(TARGETS fort-hermes DESTINATION lib)
