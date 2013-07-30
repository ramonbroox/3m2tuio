include_directories(${CMAKE_SOURCE_DIR}/libs/hidapi  ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libs)

set(TS3M2TUIO_SRCS
	main.cpp
	TS3MEventSource.cpp
	Utilities.cpp
)

if(WIN32)
else(WIN32)
	set(SYSLIBS ${SYSLIBS} pthread)
endif(WIN32)

add_executable(TS3M2TUIO ${SYS} ${TS3M2TUIO_SRCS})
target_link_libraries(TS3M2TUIO HIDAPI TUIO ${SYSLIBS})
