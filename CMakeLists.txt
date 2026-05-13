cmake_minimum_required(VERSION 4.1)

# Only declare a project when built standalone, not as a submodule.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	project(werelib LANGUAGES CXX)
endif()

# Guard against double-inclusion when added via add_subdirectory().
if(TARGET werelib)
	return()
endif()

option(WERELIB_SHARED "Build werelib as a shared library" OFF)

if(WERELIB_SHARED)
	add_library(werelib SHARED)
else()
	add_library(werelib STATIC)
endif()

target_compile_features(werelib PUBLIC cxx_std_23)

target_sources(werelib
	PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
	FILES
		src/weretype.cppm
		src/encoding/were.base64.cppm
		src/encoding/were.hex.cppm
)
