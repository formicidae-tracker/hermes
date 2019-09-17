set(SRC_FILES )
set(HDR_FILES )

set(PROTO_FILES Tag.proto
	            FrameReadout.proto
	            Header.proto
	            )

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})


install(FILES ${PROTO_HDR} ${PROTO_FILES} DESTINATION ${INCLUDE_INSTALL_DIR})

add_library(fort-hermes-cpp SHARED ${SRC_FILES} ${HDR_FILES} ${PROTO_SRC} ${PROTO_HDR})
set_target_properties(fort-hermes-cpp PROPERTIES VERSION ${VERSION_API}
                                                 SOVERSION ${VERSION_ABI})

target_link_libraries(fort-hermes-cpp ${PROTOBUF_LIBRARIES})


install(TARGETS fort-hermes-cpp DESTINATION lib)
