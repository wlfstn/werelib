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
		src/encoding/were.base32.cppm
		src/encoding/were.base64.cppm
		src/encoding/were.ascii85.cppm
		src/encoding/were.hex.cppm
)

# Runtime checks are opt-in, including when werelib is a subdirectory.
option(WERELIB_BUILD_TESTS "Build werelib tests" OFF)
if(WERELIB_BUILD_TESTS)
	enable_testing()
	add_executable(werelib_base32_tests tests/base32.cpp)
	target_link_libraries(werelib_base32_tests PRIVATE werelib)
	add_test(NAME werelib.base32 COMMAND werelib_base32_tests)
endif()
