# Werelib
Utility library and slight STL alternative for Modern C++ development.  
See the [API reference](docs/reference.md) for types, functions, and error behavior.

### Core
- weretype.cppm -- core types, casts, and template conversions.

### Encoding
- were.hex.cppm -- Hexadecimal encoding / decoding.
- were.base64.cppm -- Base64 encoding / decoding.
- were.base32.cppm -- Strict RFC4648 Base32 encoding / decoding.
- were.ascii85.cppm -- Ascii85 encoding / decoding.

### Compression
- were.huffman.cppm -- Unique byte counts for Huffman coding.
- were.deflate.cppm -- Native raw DEFLATE compression / decompression; no dependencies.


# Using werelib in your projects!
### Cmake
`tools/build.ps1` configures, builds, and tests with Ninja and Clang. Install CMake 4.1+, Ninja, and a C++26-capable Clang (including `clang-scan-deps`) on your PATH.

```powershell
./tools/build.ps1
./tools/build.ps1 -Configuration Release
```

Debug is the default. Build output goes to `build/ninja/<Configuration>`. The script can be invoked from any working directory.

(Optional if you wish to make it shared)
```cmake
set(WERELIB_SHARED ON CACHE BOOL "" FORCE)
```
The location of the directory is anywhere you choose to place it.
```cmake
add_subdirectory(src/vendor/werelib)
```
Near the end of the CmakeLists.txt file you'll also need to have something like:
```cmake
target_link_libraries(
	${PROJECT_NAME} PRIVATE
	werelib
)
```

### Git
- Add `git submodule add https://github.com/wlfstn/werelib ./stc/vendor/werelib`
- Update `git submodule update --remote --merge`
