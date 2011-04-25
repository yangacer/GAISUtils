include_directories ( ${PROJECT_SOURCE_DIR}/include/GAISUtils 
		${PROJECT_SOURCE_DIR}/loki/include
		/usr/local/include )

link_directories (${PROJECT_SOURCE_DIR}/loki/src)

# Allow the developer to select if Dynamic or Static libraries are built
OPTION (BUILD_SHARED_GUTILS "Build Shared Libraries" OFF)

# Set the LIB_TYPE variable to STATIC
SET (LIB_TYPE STATIC)

IF (BUILD_SHARED_GUTILS)
	# User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
	SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_GUTILS)


if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE)
endif(WIN32)


# Create a target for the library

add_library (GUtils ${LIB_TYPE} lexUtils.cpp fileUtils.cpp timeUtils.cpp profiler.cpp rstream.cpp rschema.cpp) 
target_link_libraries(GUtils loki)

install (TARGETS GUtils LIBRARY DESTINATION lib 
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin)

