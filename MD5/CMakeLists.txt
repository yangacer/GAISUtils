

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LIB_NAME MD5CCd)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(LIB_NAME MD5CC)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE)
endif(WIN32)


add_library(MD5CC STATIC md5.cc)

add_executable (MD5 driver.cc)
target_link_libraries(MD5 MD5CC)

if(INSTALL_MD5)
	install (FILES md5.h DESTINATION include/GAISUtils)
	install (TARGETS MD5CC LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
			RUNTIME DESTINATION lib)
endif(INSTALL_MD5)
