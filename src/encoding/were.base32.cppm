module;

#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <span>
#include <array>
#include <ranges>
#include <algorithm>

export module were.base32;
import weretype;

export namespace were::base32 {

	// [Constants]
	constexpr std::string_view Legend{"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"};
	enum struct Error { BadLength, BadChar, BadPadding };

	[[nodiscard]] constexpr
	auto numeric(char ch) -> std::expected<u8, Error> {
		const auto pos = Legend.find(ch);
		if (pos == std::string_view::npos) return std::unexpected(Error::BadChar);
		
		return as<u8>(pos);
	}

	[[nodiscard]] constexpr
	auto decode(std::string_view input)  -> std::expected<std::vector<u8>, Error> {
		if (input.empty() || input.size() % 8) return std::unexpected(Error::BadLength);

		const auto padding = std::ranges::distance(
			input
			| std::views::reverse
			| std::views::take_while([](char ch) { return ch == '='; })
		);

		if (!std::ranges::contains(std::array{0, 1, 3, 4, 6}, padding))
			return std::unexpected(Error::BadPadding);

		const auto dataSize{input.size() - padding};
		for (const auto i : were::thru(dataSize)) {
			if (input[i] == '=') return std::unexpected(Error::BadPadding);
		}

		std::vector<u8> out{};
		out.reserve(dataSize * 5 / 8);

		u32 value{}, bits{};

		for (const auto ch : input.substr(0, dataSize)) {
			const auto digit{numeric(ch)};
			if (!digit) return std::unexpected(digit.error());
			value = (value << 5) | *digit;
			bits += 5;
			if (bits >= 8) {
				bits -= 8;
				out.push_back(as<u8>(value >> bits));
			}
			value &= (1u << bits) - 1; // Keep only unused bits.
		}
		if (value) return std::unexpected(Error::BadPadding);
		return out;
	}

	[[nodiscard]] constexpr
	auto encode(std::span<const u8> input) -> std::string {
		std::string out{};
		u32 value{}, bits{};

		for (const auto byte : input) {
			value = (value << 8) | byte;
			bits += 8;
			while (bits >= 5) {
				bits -= 5;
				out.push_back(Legend[(value >> bits) & 0x1F]);
			}
			value &= (1u << bits) - 1;
		}
		if (bits) out.push_back(Legend[(value << (5 - bits)) & 0x1F]);
		while (out.size() % 8) out.push_back('=');
		return out;
	}
}

// [Static Assert Testing]
namespace were::base32::testing {
	static_assert(Legend.size() == 32);
	static_assert(numeric('A') == 0 && numeric('Z') == 25 && numeric('2') == 26 && numeric('7') == 31);
	static_assert(!numeric('=') && !numeric('a') && !numeric('0') && !numeric('1'));

	static_assert([] {
		constexpr std::array<u8, 6> bytes{'f', 'o', 'o', 'b', 'a', 'r'};
		constexpr std::array<std::string_view, 7> encoded{
			"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"
		};
		for (const auto size : were::thru(encoded.size())) {
			const auto input{std::span<const u8>{bytes}.first(size)};
			if (encode(input) != encoded[size]) return false;
			if (size) {
				const auto result{decode(encoded[size])};
				if (!result || *result != std::vector<u8>(input.begin(), input.end())) return false;
			}
		}
		return true;
	}(), "Base32 RFC 4648 vectors failed");

	static_assert([] {
		for (const auto input : {"", "M", "MY", "MZX", "MZXQ", "MZXW6", "MZXW6Y", "MZXW6YQ", "MY=====", "MY======="}) {
			const auto result{decode(input)};
			if (result || result.error() != Error::BadLength) return false;
		}
		return true;
	}(), "Base32 invalid length test failed");

	static_assert([] {
		for (const auto input : {"========", "A=======", "AAA=====", "AAAAAA==", "MY=====A", "=Y======", "MY======MZXW6YTB",
			"MZ======", "MZXR====", "MZXW7===", "MZXW6YR="}) {
			const auto result{decode(input)};
			if (result || result.error() != Error::BadPadding) return false;
		}
		return true;
	}(), "Base32 invalid padding and unused bits test failed");

	static_assert([] {
		for (const auto input : {"my======", "M0======", "M1======", "M8======", "M!======", "M ======", "M\n======", "M\t======"}) {
			const auto result{decode(input)};
			if (result || result.error() != Error::BadChar) return false;
		}
		return true;
	}(), "Base32 invalid alphabet test failed");

	static_assert([] {
		constexpr std::array<u8, 5> bytes{0x00, 0xFF, 0x80, 0x01, 0xFE};
		const auto result{decode(encode(bytes))};
		return result && *result == std::vector<u8>(bytes.begin(), bytes.end());
	}());
}
