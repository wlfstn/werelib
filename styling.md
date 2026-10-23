# Styling

## Functions

Put function attributes and `constexpr` together above the signature, after any template header. Preserve other attributes' semantic placement.

```cpp
template <class T>
[[nodiscard]] constexpr
T identity(T value) { return value; }
```

## Byte types

- `byte` = `std::byte`: raw storage.
- `u8` = `std::uint8_t`: 8-bit unsigned numbers and encoding arithmetic.