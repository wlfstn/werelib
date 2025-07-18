cmake_minimum_required(VERSION 3.16)
project(WERELIB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(werelib STATIC
	lib/artnet/artnet.cpp
	lib/win/spout.cpp
	lib/win/winsock.cpp
)

target_include_directories(werelib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/lib
	${CMAKE_CURRENT_SOURCE_DIR}/lib/artnet
	${CMAKE_CURRENT_SOURCE_DIR}/lib/win
)
