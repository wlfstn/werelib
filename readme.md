# Werelib
Non-Standard utility library for Modern C++ development.

## Core
- weretype.cppm -- core types, casts, and template conversions.

## Encoding
- were.hex.cppm -- Hexadecimal encoding / decoding.
- were.base64.cppm -- Base64 encoding / decoding.

# Using werelib in your projects!
## Cmake
(Optional if you wish to make it shared)
```cmake
set(WERELIB_SHARED ON CACHE BOOL "" FORCE)
```
The location of the directory is anywhere you choose to place it.
```cmake
add_subdirectory(src/vendor/werelib)
```

## Git
- Add `git submodule add https://github.com/wlfstn/werelib ./stc/vendor/werelib`
- Update `git submodule update --remote --merge`