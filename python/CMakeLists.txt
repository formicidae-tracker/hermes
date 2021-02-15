set(PROTO_FILES protobuf/Tag.proto
	protobuf/FrameReadout.proto
	protobuf/Header.proto
	)

protobuf_generate_python(PY_SOURCES ${PROTO_FILES})

add_custom_target(python_proto ALL 
                  DEPENDS ${PY_SOURCES})
                  
install(FILES ${PY_SOURCES} DESTINATION ${PROJECT_SOURCE_DIR}/python)
