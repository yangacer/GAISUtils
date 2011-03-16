set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../)
add_library (daemon SHARED daemon_unix.cc partialOmniORB.cpp)

