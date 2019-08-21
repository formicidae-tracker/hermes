set(SRC_FILES hermes.cpp
              Error.cpp
              NetworkContext.cpp
              FileContext.cpp)


set(HDR_FILES hermes.h
              Error.h
              Context.h
              NetworkContext.h
              FileContext.h)


install(FILES ${HDR_FILES} DESTINATION ${INCLUDE_INSTALL_DIR})

include_directories(${PROJECT_BINARY_DIR}/protobuf)

add_library(fort-hermes SHARED ${SRC_FILES} ${HDR_FILES} ${PROTO_C_HDR} ${PROTO_C_SRC})
set_target_properties(fort-hermes PROPERTIES VERSION ${VERSION_API}
                                                     SOVERSION ${VERSION_ABI})

target_link_libraries(fort-hermes fort-hermes-cpp ${Boost_LIBRARIES})


install(TARGETS fort-hermes DESTINATION lib)
