cmake_minimum_required(VERSION 4.1)

project(werelib LANGUAGES CXX)
add_library(werelib)
target_compile_features(werelib PUBLIC cxx_std_26)

target_sources(werelib PUBLIC FILE_SET CXX_MODULES FILES
	src/weretype.cppm
	src/compression/were.deflate.cppm
	src/compression/were.huffman.cppm
	src/encoding/were.base32.cppm
	src/encoding/were.base64.cppm
	src/encoding/were.ascii85.cppm
	src/encoding/were.hex.cppm
)
