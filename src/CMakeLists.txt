set(SRC_FILES hermes.cpp)


set(HDR_FILES hermes.h)

install(FILES ${HDR_FILES} DESTINATION ${INCLUDE_INSTALL_DIR})

add_library(fort-hermes SHARED ${SRC_FILES} ${HDR_FILES} ${PROTO_C_HDR} ${PROTO_C_SRC})
set_target_properties(fort-hermes PROPERTIES VERSION ${VERSION_API}
                                                     SOVERSION ${VERSION_ABI})

target_link_libraries(fort-hermes fort-hermes-cpp)

install(TARGETS fort-hermes DESTINATION lib)
