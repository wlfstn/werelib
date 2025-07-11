cmake_minimum_required(VERSION 3.16)
project(WERELIB VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

add_library(werelib STATIC
	lib/artnet/artnet.cpp
	lib/win/spout.cpp
)

target_include_directories(werelib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/WERELIBConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/WERELIBConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/WERELIBConfig.cmake"
	INSTALL_DESTINATION lib/cmake/WERELIB
)

install(TARGETS werelib
	EXPORT werelib-targets
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT werelib-targets
	FILE WERELIBTargets.cmake
	NAMESPACE werelib::
	DESTINATION lib/cmake/WERELIB
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/WERELIBConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/WERELIBConfigVersion.cmake"
	DESTINATION lib/cmake/WERELIB
)