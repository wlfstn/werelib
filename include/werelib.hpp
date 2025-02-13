#pragma once

namespace were {
	template <typename T, typename U>
	[[nodiscard]] constexpr T as(U&& value) noexcept {
		return static_cast<T>(value);
	}
}
