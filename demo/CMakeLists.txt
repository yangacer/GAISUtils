include_directories( ../include ../loki/include )
add_executable(circular_buffer container.cpp)


get_property(LIB_NAME CACHE LIB_NAME:INTERNAL PROPERTY LIB_NAME)


add_executable(coder coding.cpp)

target_link_libraries(coder ${LIB_NAME})
