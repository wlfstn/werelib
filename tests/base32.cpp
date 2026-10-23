#include <array>
#include <iostream>
#include <span>
#include <vector>
#include <string>
import weretype;
import were.base32;

int main() {
	std::array<u8, 1024> bytes{};
	for (const auto i : were::thru(bytes.size())) bytes[i] = as<u8>(i);
	for (const auto size : were::thru(bytes.size() + 1)) {
		const auto input{std::span<const u8>{bytes}.first(size)};
		const auto encoded{were::base32::encode(input)};
		const auto decoded{were::base32::decode(encoded)};
		if (!size) {
			if (!encoded.empty() || decoded || decoded.error() != were::base32::Error::BadLength) return 1;
		} else if (!decoded || *decoded != std::vector<u8>(input.begin(), input.end())) return 2;
	}
	// All byte values, each possible tail length, and non-ASCII/NUL input.
	for (const auto byte : were::thru(256)) {
		bytes.fill(as<u8>(byte));
		for (std::size_t size{1}; size <= 16; ++size) {
			const auto input{std::span<const u8>{bytes}.first(size)};
			const auto decoded{were::base32::decode(were::base32::encode(input))};
			if (!decoded || *decoded != std::vector<u8>(input.begin(), input.end())) return 3;
		}
	}
	for (const auto ch : {char{0}, as<char>(0x80), as<char>(0xFF), '\r', '\n'}) {
		auto input{std::string{"MY======"}};
		input[0] = ch;
		const auto result{were::base32::decode(input)};
		if (result || result.error() != were::base32::Error::BadChar) return 4;
	}
	for (const auto input : {"", "A", "MY", "AAA=====", "AAAAAA==", "========", "MY=====A", "MZ======", "MZXR====", "MZXW7===", "MZXW6YR=", "my======", "M0======", "M1======"}) {
		if (were::base32::decode(input)) return 5;
	}
	std::cout << "Base32: 5121 runtime round-trips and invalid-byte checks passed\n";
}
