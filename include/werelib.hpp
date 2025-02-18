#pragma once

#include <bit>
#include <array>
#include <cstddef>
#include <type_traits>

namespace were {
	using byte = unsigned char;

	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;

	using i8 = signed char;
	using i16 = signed short;
	using i32 = signed int;
	using i64 = signed long long;

	// were::as<T>(V) in lieu of static_cast<T>(V)
	template <typename ToType, typename From>
	[[nodiscard]] consteval ToType as(From&& value) noexcept {
		return static_cast<ToType>(value);
	}
	
	// were::asraw<T>(V) in lieu of reinterpret_cast<T>(V)
	template <typename ToType, typename From>
	[[nodiscard]] consteval ToType asraw(From&& value) noexcept {
		return reinterpret_cast<ToType>(value);
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
	template <typename T>
	constexpr T bigEndianSwap(T value) {
		return std::endian::native == std::endian::little
			? std::byteswap(value)
			: value;
	}

	// were::packByte(V1, V2)
	consteval u16 packBytes(byte a, byte b) {
		std::array<byte, 2> data = { a, b };
		return std::bit_cast<std::uint16_t>(data);
	}
}
