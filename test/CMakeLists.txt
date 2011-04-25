cmake_minimum_required(VERSION 2.6)

include_directories( ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/loki/include /usr/local/include)
link_directories( ${PROJECT_BINARY_DIR}/src ${PROJECT_SOURCE_DIR}/loki/src /usr/local/lib)

set(TESTS rstream_test record_test)
foreach(tg ${TESTS})
	add_executable(${tg} ${tg}.cpp)
	target_link_libraries(${tg} GUtils loki re2)
endforeach(tg)

