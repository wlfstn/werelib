module;

#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <span>
#include <ranges>
#include <cassert>

export module were.base64;
import weretype;

export namespace were::base64 {
	enum struct Error { badLength, badChar, badPadding };

	[[nodiscard]] auto asSextet(char ch)
	-> std::expected<u8, Error> {
		if (ch >= 'A' && ch <= 'Z') return as<u8>(ch - 'A');
		if (ch >= 'a' && ch <= 'z') return as<u8>(ch - 'a' + 26);
		if (ch >= '0' && ch <= '9') return as<u8>(ch - '0' + 52);
		if (ch == '+') return 62;
		if (ch == '/') return 63;

		return std::unexpected(Error::badChar);
	}

	[[nodiscard]] auto decode(std::string_view input)
	-> std::expected<std::vector<u8>, Error> {
		if (input.size() % 4 != 0) return std::unexpected(Error::badLength);
		if (input.empty()) return std::vector<u8>{};

		size_t padding{};

		if (input[input.size() - 1] == '=') ++padding;
		if (input[input.size() - 2] == '=') ++padding;

		if (padding > 2) return std::unexpected(Error::badPadding);

		// for (size_t i{}; i + padding < input.size(); ++i) {
		for (auto i : std::views::iota(0uz, input.size() - padding)) {
			if (input[i] == '=') return std::unexpected(Error::badPadding);
		}

		std::vector<u8> out;
		out.resize((input.size() / 4) * 3 - padding);

		size_t outIndex{};
		for (size_t i = 0; i < input.size(); i += 4) {
			u8 sextets[4]{};

			for (auto x : std::views::iota(0uz, 4uz)) {
				const char ch = input[i + x];
				if (ch == '=') {
					sextets[x] = 0;
					continue;
				}

				auto r = asSextet(ch);
				if (!r) return std::unexpected(r.error());

				sextets[x] = *r;
			}

			const u8 b0 = as<u8>((sextets[0] << 2) | (sextets[1] >> 4));
			const u8 b1 = as<u8>(((sextets[1] & 0x0F) << 4) | (sextets[2] >> 2));
			const u8 b2 = as<u8>(((sextets[2] & 0x03) << 6) | sextets[3]);

			if (outIndex < out.size()) out[outIndex++] = b0;
			if (outIndex < out.size()) out[outIndex++] = b1;
			if (outIndex < out.size()) out[outIndex++] = b2;
		}

		return out;
	}

	[[nodiscard]] auto encode(
		std::span<const u8> input
	) -> std::string {
		static constexpr char Encode_LUT[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		if (input.empty()) {
			return {};
		}

		const size_t fullGroups = input.size() / 3;
		const size_t remainder = input.size() % 3;

		std::string out;
		out.resize(((input.size() + 2) / 3) * 4);

		size_t inIndex{};
		size_t outIndex{};

		for (size_t i = 0; i < fullGroups; ++i) {
			const u8 b0 = input[inIndex++];
			const u8 b1 = input[inIndex++];
			const u8 b2 = input[inIndex++];

			out[outIndex++] = Encode_LUT[b0 >> 2];
			out[outIndex++] = Encode_LUT[((b0 & 0x03) << 4) | (b1 >> 4)];
			out[outIndex++] = Encode_LUT[((b1 & 0x0F) << 2) | (b2 >> 6)];
			out[outIndex++] = Encode_LUT[b2 & 0x3F];
		}

		if (remainder == 1) {
			const u8 b0 = input[inIndex];

			out[outIndex++] = Encode_LUT[b0 >> 2];
			out[outIndex++] = Encode_LUT[(b0 & 0x03) << 4];
			out[outIndex++] = '=';
			out[outIndex++] = '=';
		} else if (remainder == 2) {
			const u8 b0 = input[inIndex++];
			const u8 b1 = input[inIndex];

			out[outIndex++] = Encode_LUT[b0 >> 2];
			out[outIndex++] = Encode_LUT[((b0 & 0x03) << 4) | (b1 >> 4)];
			out[outIndex++] = Encode_LUT[(b1 & 0x0F) << 2];
			out[outIndex++] = '=';
		}

		assert(outIndex == out.size());
		return out;
	}
}