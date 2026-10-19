# Reference

## Core (`import weretype;`)
Types and cast helpers are in the global namespace. Range helpers and `Result` are in `were`.

| Type | Description |
| --- | --- |
| `byte`, `u8`, `u16`, `u32`, `u64` | Unsigned fixed-width integers; `byte` is an alias for `u8`. |
| `i8`, `i16`, `i32`, `i64` | Signed fixed-width integers. |
| `f32`, `f64` | Aliases for `float` and `double`. |
| `were::Result<T = void>` | Alias for `std::expected<T, std::string>`. |

| Function / concept | Description |
| --- | --- |
| `as<T>(value)` | Applies `static_cast<T>` with perfect forwarding. Supports constant evaluation when the cast does, and is `noexcept` when the cast is. |
| `raw<T>(value)` | Applies `reinterpret_cast<T>`. Intended for runtime use. |
| `byteRange<T>` | Requires `const T` to be a contiguous, sized range. |
| `u8span(input)` | Returns a read-only `std::span<const u8>` over a range's object bytes, with length `input.size() * sizeof(element)`. Does not copy or extend the input's lifetime. |
| `were::thru(count)` | Returns a view of indices from zero up to, but excluding, `count` (`std::size_t`). |
| `were::thru(range)` | Returns a view of `(index, element)` pairs, starting at index zero. |

## Hex (`import were.hex;`)
Functions and constants are in `were::hex`. Encoding, decoding, and character conversion are `constexpr`.

| Function | Returns | Description |
| --- | --- | --- |
| `numeric(char ch)` | `std::expected<u8, Error>` | Converts a hexadecimal digit to 0–15. Accepts uppercase and lowercase; returns `BadChar` for other characters. |
| `decode(std::string_view input)` | `std::expected<std::vector<u8>, Error>` | Converts pairs of hexadecimal digits to bytes. Empty or odd-length input returns `BadLength`; invalid digits return `BadChar`. |
| `encode(std::span<const u8> input)` | `std::string` | Produces two uppercase hexadecimal digits per byte. Empty input produces an empty string. |
| `NormalizedHex(char ch)` | `char` | Converts lowercase `a`–`f` to uppercase; leaves other characters unchanged. |

`Error` contains `BadLength` and `BadChar`. `Legend` holds `0123456789ABCDEF`; `CaseOffset` is the difference between `'a'` and `'A'`.

## Base64 (`import were.base64;`)
Functions and constants are in `were::base64`. Encoding, decoding, and character conversion are `constexpr`.

| Function | Returns | Description |
| --- | --- | --- |
| `numeric(char ch)` | `std::expected<u8, Error>` | Converts a character from the Base64 alphabet to 0–63. Returns `BadChar` for other characters, including `=`. |
| `decode(std::string_view input)` | `std::expected<std::vector<u8>, Error>` | Decodes Base64 with optional trailing `=` padding. Empty input or a length not divisible by four returns `BadLength`; misplaced or excessive padding returns `BadPadding`; invalid alphabet characters return `BadChar`. |
| `encode(std::span<const u8> input)` | `std::string` | Encodes bytes using the standard Base64 alphabet and adds `=` padding as needed. Empty input produces an empty string. |

`Error` contains `BadLength`, `BadChar`, and `BadPadding`. `Legend` holds the standard alphabet: `A–Z`, `a–z`, `0–9`, `+`, `/`. Whitespace and URL-safe `-` / `_` characters are not accepted by the decoder.
