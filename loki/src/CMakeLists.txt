include_directories( ${PROJECT_SOURCE_DIR}/include )

set (sources LevelMutex.cpp
	OrderedStatic.cpp
	SafeFormat.cpp
	Singleton.cpp
	SmallObj.cpp
	SmartPtr.cpp
	StrongPtr.cpp)

add_library( loki STATIC ${sources} )

install (TARGETS loki ARCHIVE DESTINATION lib )

