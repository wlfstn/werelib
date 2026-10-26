module;

#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <span>
#include <array>
#include <algorithm>
#include <limits>
#include <ranges>

export module were.ascii85;
import weretype;

// Raw Ascii85: no Adobe framing or whitespace.
export namespace were::ascii85 {

	// [Constants]
	constexpr std::string_view Legend{
		R"(!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstu)"
	};
	constexpr char ZeroGroup{'z'}; // Abbreviates a full binary group of four zero bytes.
	enum struct Error { BadLength, BadChar, Overflow };

	[[nodiscard]] constexpr
	auto numeric(char ch) -> std::expected<u8, Error> {
		const auto pos = Legend.find(ch);
		if (pos == std::string_view::npos) return std::unexpected(Error::BadChar);

		return as<u8>(pos);
	}

	[[nodiscard]] constexpr
	auto zeroPositions(std::string_view input) {
		std::vector<std::size_t> positions;
		for (const auto [i, ch] : were::thru(input)) {
			if (ch == ZeroGroup) positions.push_back(i);
		}
		return positions;
	}

	[[nodiscard]] constexpr
	auto decode(std::string_view input) -> std::expected<std::vector<u8>, Error> {
		if (input.empty()) return std::unexpected(Error::BadLength);

		std::vector<u8> out{};
		u64 value{};
		std::size_t count{};

		for (const auto ch : input) {
			if (ch == ZeroGroup) {
				if (count) return std::unexpected(Error::BadChar);
				out.insert(out.end(), 4, u8{});
				continue;
			}

			const auto digit{numeric(ch)};
			if (!digit) return std::unexpected(digit.error());
			value = value * 85 + *digit;
			if (++count == 5) {
				if (value > std::numeric_limits<u32>::max()) return std::unexpected(Error::Overflow);
				for (const auto i : were::thru(4)) {
					out.push_back(as<u8>(value >> (24 - i * 8)));
				}
				value = 0;
				count = 0;
			}
		}

		if (count == 1) return std::unexpected(Error::BadLength);
		if (count) {
			// Fill missing digits with 'u'; discard their bytes below.
			for ([[maybe_unused]] const auto i : were::thru(5 - count)) {
				value = value * 85 + 84;
			}
			if (value > std::numeric_limits<u32>::max()) return std::unexpected(Error::Overflow);
			for (const auto i : were::thru(count - 1)) {
				out.push_back(as<u8>(value >> (24 - i * 8)));
			}
		}
		return out;
	}

	[[nodiscard]] constexpr
	auto encode(std::span<const u8> input) -> std::string {
		std::string out{};

		for (const auto bytes : input | std::views::chunk(4)) {
			u32 value{};

			for (const auto [i, byte] : were::thru(bytes)) {
				value |= as<u32>(byte) << (24 - i * 8);
			}

			if (bytes.size() == 4 && value == 0) {
				out.push_back(ZeroGroup);
				continue;
			}

			std::array<char, 5> group{};

			for (auto& digit : group | std::views::reverse) {
				digit = Legend[value % 85];
				value /= 85;
			}
			out.append(group.data(), bytes.size() + 1);
		}

		return out;
	}
}

// [Static Assert Testing]
namespace were::ascii85::testing {
	static_assert([] {
		constexpr std::string_view text{"Hello world"};
		return encode(std::vector<u8>(text.begin(), text.end())) == "87cURD]j7BEbo7";
	}(), "Ascii85 Hello world encode failed");

	static_assert([] {
		const auto result{decode("87cURD]j7BEbo7")};
		return result && std::ranges::equal(*result, std::string_view{"Hello world"});
	}(), "Ascii85 Hello world decode failed");

	static_assert([] {
		constexpr std::array<u8, 6> bytes{'f', 'o', 'o', 'b', 'a', 'r'};
		constexpr std::array<std::string_view, 7> encoded{
			"", "Ac", "Ao@", "AoDS", "AoDTs", "AoDTs@/", "AoDTs@<)"
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
	}(), "Ascii85 known vectors failed");

	static_assert(
		decode("").error() == Error::BadLength &&
		decode("!").error() == Error::BadLength &&
		decode("!v").error() == Error::BadChar &&
		decode("!z").error() == Error::BadChar &&
		decode("uuuuu").error() == Error::Overflow &&
		decode("uu").error() == Error::Overflow,
		"Ascii85 invalid input checks failed"
	);

	static_assert([] {
		constexpr std::array<u8, 5> bytes{};
		const auto result{decode("z!!")};
		return encode(bytes) == "z!!" && result &&
			*result == std::vector<u8>(bytes.begin(), bytes.end()) &&
			decode("!!!!!") == decode("z");
	}(), "Ascii85 zero groups failed");

	static_assert([] {
		constexpr std::array<u8, 7> bytes{255, 255, 255, 255, 0, 128, 1};
		const auto result{decode(encode(bytes))};
		return encode(std::span<const u8>{bytes}.first(4)) == "s8W-!" &&
			result && *result == std::vector<u8>(bytes.begin(), bytes.end());
	}(), "Ascii85 binary round-trip failed");

}
