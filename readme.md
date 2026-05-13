# Werelib
Non-Standard utility library for Modern C++ development.

## Core
- weretype.cppm -- core types, casts, and template conversions.

## Encoding
- were.hex.cppm -- Hexadecimal encoding / decoding.
- were.base64.cppm -- Base64 encoding / decoding.

# Adding to projects
Using werelib in your projects!
## Cmake
```cmake
set(WERELIB_SHARED ON CACHE BOOL "" FORCE)
add_subdirectory(werelib)
```

## Git
- Add `git submodule add https://github.com/wlfstn/werelib ./stc/vendor/werelib`
- Update `git submodule update --remote --merge`