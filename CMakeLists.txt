project(3M2TUIO)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_OSX_ARCHITECTURES x86_64)

set(3M2TUIO_VERSION_MAJOR 0)
set(3M2TUIO_VERSION_MINOR 1)

if(APPLE)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(FOUNDATION_LIBRARY CoreFoundation)
	set(MAC_LIBS ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(WIN32)
	find_library(SETUPAPI_LIBRARY setupapi)
	set(WIN_LIBS ${SETUPAPI_LIBRARY})	
endif(APPLE)

add_subdirectory(src)
add_subdirectory(libs/TUIO)
find_package(HIDAPI)
