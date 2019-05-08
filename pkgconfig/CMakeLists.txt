set(PKG_FILES fort-hermes.pc)

foreach(p ${PKG_FILES})
	configure_file(${p}.in ${p} @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${p}
		    DESTINATION lib/pkgconfig
		    )
endforeach(p ${PKG_FILES})
