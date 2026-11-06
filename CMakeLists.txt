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
option(WERELIB_BUILD_TESTS "Build werelib tests" OFF)

if(WERELIB_SHARED)
	add_library(werelib SHARED)
else()
	add_library(werelib STATIC)
endif()

target_compile_features(werelib PUBLIC cxx_std_26)

target_sources(werelib
	PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
	FILES
		src/weretype.cppm
		src/compression/were.deflate.cppm
		src/compression/were.huffman.cppm
		src/encoding/were.base32.cppm
		src/encoding/were.base64.cppm
		src/encoding/were.ascii85.cppm
		src/encoding/were.hex.cppm
)

if(WERELIB_BUILD_TESTS)
	enable_testing()
	add_executable(werelib_huffman_tests tests/huffman.cpp)
	target_link_libraries(werelib_huffman_tests PRIVATE werelib)
	add_test(NAME werelib_huffman_tests COMMAND werelib_huffman_tests)
	add_executable(werelib_deflate_tests tests/deflate.cpp)
	target_link_libraries(werelib_deflate_tests PRIVATE werelib)
	add_test(NAME werelib_deflate_tests COMMAND werelib_deflate_tests)
endif()
