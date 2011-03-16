cmake_minimum_required(VERSION 2.6)

include_directories( ${PROJECT_SOURCE_DIR}/include)
link_directories( ${PROJECT_BINARY_DIR}/src)

set(TESTS rstream_test)
foreach(tg ${TESTS})
	add_executable(${tg} ${tg}.cpp)
	target_link_libraries(${tg} GUtils)
endforeach(tg)

