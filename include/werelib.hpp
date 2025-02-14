#pragma once

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

	template <typename T, typename U>
	[[nodiscard]] constexpr T as(U&& value) noexcept {
		return static_cast<T>(value);
	}
}
