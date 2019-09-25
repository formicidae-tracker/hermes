set(SRC_FILES )
set(HDR_FILES )

set(PROTO_FILES Tag.proto
	            FrameReadout.proto
	            Header.proto
	            )

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILES})


install(FILES ${PROTO_HDR} ${PROTO_FILES} DESTINATION ${INCLUDE_INSTALL_DIR})

set(PROTO_HDR ${PROTO_HDR} PARENT_SCOPE)
set(PROTO_SRC ${PROTO_SRC} PARENT_SCOPE)
