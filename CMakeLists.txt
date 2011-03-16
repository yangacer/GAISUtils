cmake_minimum_required (VERSION 2.6)
project (GAISUtilityLib) 
#add_subdirectory (daemon)

add_subdirectory (MD5)
add_subdirectory (loki)
add_subdirectory (src)
add_subdirectory (include)
add_subdirectory (test)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE)
endif(WIN32)


if(NOT CMAKE_INSTALL_PREFIX )
	if(WIN32)
		set(CMAKE_INSTALL_PREFIX c:/NDB)
	else(WIN32)
		set(CMAKE_INSTALL_PREFIX /usr/local)
	endif(WIN32)
endif(NOT CMAKE_INSTALL_PREFIX)

option(INSTALL_MD5 "A 3-rd party MD5 tool" ON)



