module;

#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <span>
#include <cassert>

export module were.hex;
import weretype;

export namespace were::hex {
	enum struct Error { invalidLength, invalidChar};

	[[nodiscard]] auto asNybble(char ch) 
	-> std::expected<u8, Error> {
		if (ch >= '0' && ch <= '9') return as<u8>(ch - '0');
		if (ch >= 'a' && ch <= 'f') return as<u8>(ch - 'a' + 10);
		if (ch >= 'A' && ch <= 'F') return as<u8>(ch - 'A' + 10);
		return std::unexpected(Error::invalidChar);
	}

	[[nodiscard]] auto decode(std::string_view input) 
	-> std::expected<std::vector<u8>, Error> {
		if (input.size() % 2 != 0) return std::unexpected(Error::invalidLength);
		std::vector<u8> out(input.size() / 2);

		for (auto [i, byte] : were::thru(out)) {
			for (auto x : {0,1}) {
				auto r = asNybble(input[i*2+x]);
				if (!r) return std::unexpected(r.error());
				out[i] |= (*r << (4-(x*4)));
			}	
		}
		return out;
	}

	[[nodiscard]] auto encode(std::span<const u8> input) 
	-> std::string {
		static constexpr char Encode_LUT[] = "0123456789ABCDEF";
		std::string out(input.size() * 2, '\0');

		for (auto [i, elem] : were::thru(input)) {
			size_t indx = i*2;
			assert(indx + 1 < out.size());
			out[indx] = Encode_LUT[elem >> 4];
			out[indx + 1] = Encode_LUT[elem & 0x0F];
		}

		return out;
	}
}
