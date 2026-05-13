# Werelib
Non-Standard utility library for Modern C++ development.

## Core
- weretype.cppm -- core types, casts, and template conversions.

## Encoding
- were.hex.cppm -- Hexadecimal encoding / decoding.
- were.base64.cppm -- Base64 encoding / decoding.

# Adding to projects
```cmake
set(WERELIB_SHARED ON CACHE BOOL "" FORCE)
add_subdirectory(werelib)
```