## Header Library 
### Werelib.hpp

| Category     | Types                       |
|--------------|-----------------------------|
| Unsigned int | `u8`, `u16`, `u32`, `u64`   |
| Signed int   | `i8`, `i16`, `i32`, `i64`   |
| Float        | `f32`, `f64`                |
| Alias        | `byte`                      |

| Header Library  			| Description                                 | 
|-----------------------|---------------------------------------------|
| `were::as`      			| Type Casing : Type Safe                     |
| `were::asraw`   			| Type punning : Type Safe & defined behavior |
| `were::asBytes` 			| Union of Type & modern C++ array of bytes   |
| `were::bigEndianSwap` | Conditional: Convert LE to BE if LE system  |