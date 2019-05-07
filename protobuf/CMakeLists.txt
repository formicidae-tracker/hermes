set(SRC_FILES )
set(HDR_FILES )

set(PROTO_FILES Ant.proto
	            FrameReadout.proto
	            )


protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})


install(FILES ${PROTO_HDR} DESTINATION ${INCLUDE_DIR})

add_library(hermes STATIC ${SRC_FILES} ${HDR_FILES} ${PROTO_SRC} ${PROTO_HDR})

target_link_libraries(hermes ${PROTOBUF_LIBRARIES})


install(TARGETS hermes DESTINATION lib)
