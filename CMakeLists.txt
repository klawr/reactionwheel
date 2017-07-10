cmake_minimum_required(VERSION 3.2.2)
project(iot)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "in-source builds are not supported!")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

##########################################################################
# library includes

# boost library
find_package(Boost REQUIRED)

# protobuf library
find_package(Protobuf REQUIRED)

# ncurses library
find_package(Curses REQUIRED)

# wiring library
find_package(wiring REQUIRED)

##########################################################################
# compiler adjustments

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -Wpedantic -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wpedantic -std=c++1z")

########################################################################
add_subdirectory(src)

