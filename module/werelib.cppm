module;
#include <bit>
#include <array>
#include <cstddef>
#include <type_traits>

//Macros must be before the export module <name>
export module werelib;

export namespace were {
	using byte = unsigned char;

	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;

	using i8 = signed char;
	using i16 = signed short;
	using i32 = signed int;
	using i64 = signed long long;

	using f32 = float;
	using f64 = double;

	// were::as<T>(V)
	// static_cast alias at compile time
	template <typename ToType, typename From>
	[[nodiscard]] constexpr ToType as(From&& value) noexcept {
		return static_cast<ToType>(value);
	}
	
	// were::asraw<T>(V)
	// reinterpret_cast without undefined behavior alias at compile time
	template <typename ToType, typename From>
	[[nodiscard]] constexpr ToType asraw(From&& value) noexcept {
		return std::bit_cast<ToType>(value);
	}

	// were::asBytes(V);
	template <typename T>
	[[nodiscard]] constexpr auto asBytes(const T& value) noexcept -> std::array<byte, sizeof(T)> {
		static_assert(std::is_trivially_copyable_v<T>, "to_bytes requires a trivially copyable type");
		union {
			T input;
			std::array<byte, sizeof(T)> output;
		} u = { value };
		return u.output;
	}
	
	// were::bigEndianSwap(V) 
	// Convert little endian to big endian (only on little endian machines)
	template <typename T>
	constexpr T bigEndianSwap(T value) {
		return std::endian::native == std::endian::little
			? std::byteswap(value)
			: value;
	}
}
