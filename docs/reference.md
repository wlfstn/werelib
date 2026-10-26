# Reference

## Core

`import weretype;`

Types and casts are global; range helpers and `Result` are in `were`.

| Type | Meaning |
| --- | --- |
| `byte` | `std::byte` for raw storage |
| `u8`, `u16`, `u32`, `u64` | Unsigned fixed-width integers |
| `i8`, `i16`, `i32`, `i64` | Signed fixed-width integers |
| `f32`, `f64` | `float`, `double` |
| `were::Result<T = void>` | `std::expected<T, std::string>` |

| Helper | Behavior |
| --- | --- |
| `as<T>(value)` | Forwarding `static_cast<T>`; conditionally `noexcept` |
| `raw<T>(value)` | Runtime `reinterpret_cast<T>` |
| `byteRange<T>` | Requires `const T` to be a contiguous, sized range |
| `u8span(input)` | Views object bytes as `std::span<const u8>`; input must outlive the view |
| `were::thru(count)` | Index view over `[0, count)` |
| `were::thru(range)` | View of `(index, element)` pairs, starting at zero |

## Encodings

| Format | Import | Namespace | Alphabet |
| --- | --- | --- | --- |
| Hex | `import were.hex;` | `were::hex` | `0-9`, `A-F` (case-insensitive) |
| Base64 | `import were.base64;` | `were::base64` | `A-Z`, `a-z`, `0-9`, `+`, `/` |
| Base32 | `import were.base32;` | `were::base32` | `A-Z`, `2-7` |
| Ascii85 | `import were.ascii85;` | `were::ascii85` | `!` through `u`; `z` abbreviates four zero bytes |

All encodings expose these `constexpr` functions and an alphabet constant, `Legend`:

| Function | Returns | Behavior |
| --- | --- | --- |
| `numeric(char)` | `std::expected<u8, Error>` | Alphabet index; otherwise `BadChar` |
| `decode(std::string_view)` | `std::expected<std::vector<u8>, Error>` | Decoded bytes or an error |
| `encode(std::span<const u8>)` | `std::string` | Encoded text; empty input produces an empty string |

All decoders reject empty input and whitespace. Base64 and Base32 add trailing `=` padding as needed; their `numeric('=')` returns `BadChar`.

### Hex

Encodes two uppercase digits per byte.

- `BadLength`: empty or odd-length input.
- `BadChar`: invalid hex digit.

`NormalizedHex(char)` uppercases `a-f`, leaving other characters unchanged. `CaseOffset` equals `'a' - 'A'`.

### Base64

Uses the standard alphabet; URL-safe `-` and `_` are rejected.

- `BadLength`: empty input or length not divisible by four.
- `BadPadding`: misplaced `=` or more than two padding characters.
- `BadChar`: invalid alphabet character.

### Base32

Uses standard RFC 4648 Base32; lowercase and alternate alphabets are rejected.

- `BadLength`: empty input or length not divisible by eight.
- `BadPadding`: misplaced `=`, padding count outside `{0, 1, 3, 4, 6}`, or nonzero unused bits.
- `BadChar`: invalid alphabet character.

Validation order: length, padding structure, alphabet, unused bits. Partial groups require padding.

```cpp
#include <array>
import weretype;
import were.base32;

constexpr std::array<u8, 3> bytes{'f', 'o', 'o'};
auto text = were::base32::encode(bytes); // "MZXW6==="
auto result = were::base32::decode(text);
if (result) { /* use *result */ }
```

### Ascii85

Uses raw Ascii85: four bytes become five characters; partial groups use two to four characters. Full groups of four zero bytes encode as `z`; partial zero groups use ordinary digits. No padding markers or Adobe `<~ ~>` framing. Whitespace and the btoa `y` abbreviation are rejected.

- `BadLength`: empty input or a final group containing only one digit.
- `BadChar`: invalid alphabet character or `z` inside a digit group.
- `Overflow`: a group exceeds the 32-bit range.

## Tests

Base32 checks cover RFC 4648 vectors, invalid input, padding bits, and binary round-trips.
Ascii85 compile-time checks cover known vectors, invalid input, zero groups, and binary round-trips.
With CMake 4.1+ and a C++23 module-capable compiler:

```sh
cmake -S . -B build -DWERELIB_BUILD_TESTS=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```
