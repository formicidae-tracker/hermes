find_program(CLDOC_EXE cldoc)

if(CLDOC_EXE)
	add_custom_target(docs
	                 COMMAND ${CLDOC_EXE} generate -- --output ${PROJECT_SOURCE_DIR}/docs/ --language c --merge docs/cldoc src/hermes.h
	                 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif(CLDOC_EXE)
