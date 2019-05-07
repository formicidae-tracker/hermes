set(SRC_FILES )
set(HDR_FILES )

set(PROTO_FILES Ant.proto
	            FrameReadout.proto
	            )


protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})


install(FILES ${PROTO_HDR} DESTINATION ${INCLUDE_DIR})

add_library(hermes SHARED ${SRC_FILES} ${HDR_FILES} ${PROTO_SRC} ${PROTO_HDR})
set_target_properties(hermes PROPERTIES VERSION ${VERSION_API}
                                        SOVERSION ${VERSION_ABI})

target_link_libraries(hermes ${PROTOBUF_LIBRARIES})


install(TARGETS hermes DESTINATION lib)
