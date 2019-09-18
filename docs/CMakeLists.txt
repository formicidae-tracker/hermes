find_program(CLDOC_EXE cldoc REQUIRED)

if(CLDOC_EXE)
	add_custom_target(docs
	                 COMMAND ${CLDOC_EXE} generate -- --output ${PROJECT_SOURCE_DIR}/docs/html --language c --merge docs src/hermes.h
	                 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif(CLDOC_EXE)
