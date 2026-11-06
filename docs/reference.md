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

Hex, Base64, Base32, and Ascii85 expose these `constexpr` functions and an alphabet constant, `Legend`:

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

See the [short ASCII85 history and overview](ascii85.md).

Uses raw Ascii85: four bytes become five characters; partial groups use two to four characters. Full groups of four zero bytes encode as `z`; partial zero groups use ordinary digits. No padding markers or Adobe `<~ ~>` framing. Whitespace and the btoa `y` abbreviation are rejected.

- `BadLength`: empty input or a final group containing only one digit.
- `BadChar`: invalid alphabet character or `z` inside a digit group.
- `Overflow`: a group exceeds the 32-bit range.

## Huffman

See the [short Huffman coding overview](huffman.md).

`import were.huffman;` exposes the byte counting and tree construction APIs in `were::huffman`.

`countBytes(std::span<const u8> input)` returns a `std::vector<ByteCount>` with one entry per unique byte, sorted by byte value. Each entry contains `byteval` (the byte) and `count` (its occurrence count). Empty input returns an empty vector. The function is `constexpr`.

`buildTree(std::span<const ByteCount> counts)` and `buildTree(std::span<const u8> input)` return `std::expected<Tree, Error>`. A minimum `std::priority_queue` repeatedly merges the two least frequent nodes. Equal frequencies are ordered by node index; leaves are created in byte order, so input ordering does not affect the tree.

`Tree` owns a `nodes` vector and a `root` index. Each `Node` contains `byteval`, `count`, `left`, and `right`. Child links are indices into the vector; leaves have both links set to `NoNode` and can be identified with `isLeaf()`. Only leaves have a meaningful `byteval`. A single distinct byte produces a single leaf root. A default-constructed tree has no nodes and a `NoNode` root.

Zero counts are ignored. Empty input or all-zero counts return `EmptyTree`; duplicate byte entries return `BadSymbol`; a total count exceeding `std::size_t` returns `CountOverflow`. Tree construction is declared `constexpr`, but constant evaluation requires a standard library with constexpr `std::priority_queue` support. These functions construct the tree only; they do not encode or decode data.

## DEFLATE

`import were.deflate;` (included by default; imports `weretype`, `were.huffman`, and `were.hex`; no external libraries).

| Function | Behavior |
| --- | --- |
| `were::deflate::compress(input, level = -1)` | `-1` selects level 6; `0` stores bytes; `1-9` increases match-search effort |
| `were::deflate::decompress(input, maxOutput = SIZE_MAX)` | Decompresses one complete stream, optionally limiting output bytes |

Both are `constexpr`, accept `std::span<const u8>`, and return `std::expected<std::vector<u8>, Error>`. Raw DEFLATE has no zlib/gzip framing or checksum; PNG needs a zlib wrapper.

Compression uses a 32 KiB LZ77 window and fixed Huffman codes, falling back to stored blocks when smaller. Decompression supports stored, fixed, and dynamic blocks. Compression levels are library-specific; output need not match zlib byte-for-byte.

Canonical decoding is shared with `were::huffman::CanonicalTree<SymbolCount, MaxBits = 15>`. Its `build(lengths, complete = false, allowEmpty = false)` returns `std::expected<void, were::huffman::Error>`. It rejects excessive lengths, oversubscribed codes, and incomplete codes except for a single one-bit symbol; `complete` also rejects that exception. An empty table is accepted only with `allowEmpty`. A failed build invalidates the previous table. `decode(readBit)` returns a symbol index and takes a callback returning `std::expected<u8, were::huffman::Error>` for each bit. DEFLATE maps these errors to its own error enum. Hex decoding supplies readable binary fixtures for compile-time checks.

Empty data compresses to a valid stream. Empty compressed input is truncated. Trailing bytes are rejected.
Errors: `BadLevel`, `BadData`, `TruncatedInput`, `TrailingData`, `OutputLimit`. Allocation failures throw standard exceptions.

```cpp
auto packed = were::deflate::compress(u8span(text));
if (packed) {
    auto restored = were::deflate::decompress(*packed, text.size());
}
```

## Tests

Huffman runtime checks cover byte counts, empty and single-symbol inputs, duplicate symbols, count overflow, a known optimal merge cost, tree links, and deterministic construction across all 256 byte values.

Base32 checks cover RFC 4648 vectors, invalid input, padding bits, and binary round-trips.
Ascii85 compile-time checks cover known vectors, invalid input, zero groups, and binary round-trips.
DEFLATE compile-time checks cover stored and fixed vectors, invalid input, and output limits.
Runtime checks also cover every compression level, empty input, repeated bytes, and binary data exceeding one stored block. Huffman checks include canonical table validation and decoding.
With CMake 4.1+, Ninja, and C++26-capable Clang (including `clang-scan-deps`) on PATH:

```powershell
./tools/build.ps1
```
