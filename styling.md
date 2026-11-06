# Styling

## Functions

Use trailing return types for non-void functions and lambdas: `auto name(...) -> Type`.
For functions that return no value, use `void` as the return type instead of `auto ... -> void`.
Put function attributes and `constexpr` together above the signature. Preserve other attributes' semantic placement.

```cpp
template <class T>
[[nodiscard]] constexpr
auto identity(T value) -> T { return value; }
```

```cpp
constexpr
void write(u32 number, u32 count) {
    // ...
}
```

## Byte types

- `byte` = `std::byte`: raw storage.
- `u8` = `std::uint8_t`: 8-bit unsigned numbers and encoding arithmetic.