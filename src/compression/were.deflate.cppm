module;

#include <algorithm>
#include <array>
#include <expected>
#include <span>
#include <utility>
#include <vector>

export module were.deflate;
import weretype;
import were.huffman;
import were.hex;

namespace were::deflate {
	export enum struct Error { BadLevel, BadData, TruncatedInput, TrailingData, OutputLimit };

	// DEFLATE length and distance codes, indexed by their wire symbol.
	constexpr auto LengthBits = std::to_array<u32>({
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
	});
	constexpr auto LengthBase = std::to_array<u32>({
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
	});
	constexpr auto DistanceBits = std::to_array<u32>({
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
	});
	constexpr auto DistanceBase = std::to_array<u32>({
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
	});

	constexpr auto Missing{were::numeric<std::size_t>::max};

	struct Reader {
		std::span<const u8> input;
		std::size_t position{};
		u32 value{};
		u32 bits{};
		bool failed{};

		[[nodiscard]] constexpr
		auto read(u32 count) -> u32 {
			while (bits < count) {
				if (position == input.size()) {
					failed = true;
					return 0;
				}
				value |= as<u32>(input[position++]) << bits;
				bits += were::memsize<u8>::bits;
			}

			const auto result{value & ((1u << count) - 1)};
			value >>= count;
			bits -= count;
			return result;
		}
	};

	struct Writer {
		std::vector<u8> out;
		u32 value{};
		u32 bits{};

		constexpr
		void write(u32 number, u32 count) {
			value |= number << bits;
			bits += count;

			while (bits >= were::memsize<u8>::bits) {
				out.push_back(as<u8>(value));
				value >>= were::memsize<u8>::bits;
				bits -= were::memsize<u8>::bits;
			}
		}

		constexpr
		void code(u32 number, u32 count) {
			u32 reversed{};

			for (const auto i : were::thru(count)) {
				reversed = (reversed << 1) | ((number >> i) & 1);
			}
			write(reversed, count);
		}

		constexpr
		void literal(u32 symbol) {
			if (symbol < 144) code(0x30 + symbol, 8);
			else if (symbol < 256) code(0x190 + symbol - 144, 9);
			else if (symbol < 280) code(symbol - 256, 7);
			else code(0xc0 + symbol - 280, 8);
		}
	};

	using Huffman = were::huffman::CanonicalTree<288, 15>;

	[[nodiscard]] constexpr
	auto decodeSymbol(const Huffman& tree, Reader& reader) -> std::expected<u32, Error> {
		const auto result = tree.decode([&]() -> std::expected<u8, were::huffman::Error> {
			const auto bit = as<u8>(reader.read(1));
			if (reader.failed) return std::unexpected(were::huffman::Error::TruncatedInput);
			return bit;
		});
		if (!result) return std::unexpected(result.error() == were::huffman::Error::TruncatedInput
			? Error::TruncatedInput : Error::BadData);
		return *result;
	}

	constexpr
	void fixedTrees(Huffman& literals, Huffman& distances) {
		std::array<u8, 288> lengths{};

		for (auto [i, length] : were::thru(lengths)) {
			length = i < 144 ? 8 : i < 256 ? 9 : i < 280 ? 7 : 8;
		}
		// Fixed code lengths are valid by construction.
		(void)literals.build(lengths);

		std::array<u8, 32> distanceLengths{};
		distanceLengths.fill(5);
		(void)distances.build(distanceLengths);
	}

	[[nodiscard]] constexpr
	auto dynamicTrees(Reader& reader, Huffman& literals, Huffman& distances) -> std::expected<void, Error> {
		const auto literalCount{reader.read(5) + 257};
		const auto distanceCount{reader.read(5) + 1};
		const auto codeCount{reader.read(4) + 4};
		if (reader.failed) return std::unexpected(Error::TruncatedInput);
		if (literalCount > 286) return std::unexpected(Error::BadData);

		// RFC 1951, 3.2.7 specifies this wire order for code lengths.
		constexpr auto Order = std::to_array<u8>({
			16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
		});

		std::array<u8, Order.size()> codeLengths{};

		for (const auto i : were::thru(codeCount)) {
			codeLengths[Order[i]] = as<u8>(reader.read(3));
		}
		if (reader.failed) return std::unexpected(Error::TruncatedInput);

		Huffman codes{};
		if (!codes.build(codeLengths, true)) return std::unexpected(Error::BadData);

		std::array<u8, 318> lengths{};
		const auto count{literalCount + distanceCount};

		for (u32 i{}; i < count;) {
			const auto symbol{decodeSymbol(codes, reader)};
			if (!symbol) return std::unexpected(symbol.error());
			if (*symbol < 16) {
				lengths[i++] = as<u8>(*symbol);
				continue;
			}
			if (*symbol == 16 && !i) return std::unexpected(Error::BadData);

			const auto length{*symbol == 16 ? lengths[i - 1] : u8{0}};
			const auto repeat{
				*symbol == 16 ? reader.read(2) + 3 :
				*symbol == 17 ? reader.read(3) + 3 :
				reader.read(7) + 11
			};
			if (reader.failed) return std::unexpected(Error::TruncatedInput);
			if (repeat > count - i) return std::unexpected(Error::BadData);

			std::ranges::fill(std::span{lengths}.subspan(i, repeat), length);
			i += repeat;
		}
		if (!lengths[256] || !literals.build(std::span{lengths}.first(literalCount)) ||
			!distances.build(std::span{lengths}.subspan(literalCount, distanceCount), false, true))
			return std::unexpected(Error::BadData);

		return {};
	}

	[[nodiscard]] constexpr
	auto stored(std::span<const u8> input) -> std::vector<u8> {
		std::vector<u8> out{};
		do {
			const auto count{std::min(input.size(), std::size_t{65535})};
			out.push_back(count == input.size() ? 1 : 0);
			out.push_back(as<u8>(count));
			out.push_back(as<u8>(count >> 8));
			out.push_back(as<u8>(~count));
			out.push_back(as<u8>((~count) >> 8));
			out.insert(out.end(), input.begin(), input.begin() + count);
			input = input.subspan(count);
		} while (!input.empty());

		return out;
	}

	export [[nodiscard]] constexpr
	auto compress(std::span<const u8> input, int level = -1) -> std::expected<std::vector<u8>, Error> {
		if (level < -1 || level > 9) return std::unexpected(Error::BadLevel);
		if (!level) return stored(input);
		if (input.empty()) return std::vector<u8>{3, 0};
		if (level == -1) level = 6;

		Writer writer{};
		writer.write(3, 3); // Final block, fixed Huffman codes.

		std::vector<std::size_t> heads(65536, Missing);
		std::vector<std::size_t> previous(32768, Missing);

		const auto hash = [&](std::size_t i) -> u32 {
			return ((as<u32>(input[i]) * 251 + input[i + 1]) * 251 + input[i + 2]) & 65535;
		};

		for (std::size_t position{}; position < input.size();) {
			std::size_t length{};
			std::size_t distance{};
			if (input.size() - position >= 3) {
				auto candidate{heads[hash(position)]};
				const auto maximum{std::min(input.size() - position, std::size_t{258})};

				for (u32 attempts{};
					candidate != Missing && position - candidate <= 32768 && attempts < (1u << level);
					++attempts) {
					std::size_t matched{};

					while (matched < maximum && input[candidate + matched] == input[position + matched]) {
						++matched;
					}
					if (matched > length) {
						length = matched;
						distance = position - candidate;
					}
					if (length == maximum) break;
					candidate = previous[candidate % 32768];
				}
			}

			const auto consumed{length >= 3 ? length : 1};
			if (length < 3) {
				writer.literal(input[position]);
			}
			else {
				std::size_t code{};

				while (code + 1 < LengthBase.size() && LengthBase[code + 1] <= length) {
					++code;
				}
				writer.literal(as<u32>(257 + code));
				writer.write(as<u32>(length - LengthBase[code]), LengthBits[code]);

				code = 0;
				while (code + 1 < DistanceBase.size() && DistanceBase[code + 1] <= distance) {
					++code;
				}
				writer.code(as<u32>(code), 5);
				writer.write(as<u32>(distance - DistanceBase[code]), DistanceBits[code]);
			}

			for (const auto end{position + consumed}; position < end; ++position) {
				if (input.size() - position < 3) continue;
				const auto key{hash(position)};
				previous[position % 32768] = heads[key];
				heads[key] = position;
			}
		}

		writer.literal(256);
		if (writer.bits) writer.out.push_back(as<u8>(writer.value));

		// Stored blocks avoid expanding incompressible input with Huffman codes.
		const auto blocks{input.size() / 65535 + (input.size() % 65535 != 0)};
		if (writer.out.size() > input.size() && (writer.out.size() - input.size()) / 5 >= blocks)
			return stored(input);

		return std::move(writer.out);
	}

	export [[nodiscard]] constexpr
	auto decompress(std::span<const u8> input, std::size_t maxOutput = were::numeric<std::size_t>::max)
		-> std::expected<std::vector<u8>, Error> {
		Reader reader{input};
		std::vector<u8> out{};
		bool final{};

		while (!final) {
			final = reader.read(1) != 0;
			const auto type{reader.read(2)};
			if (reader.failed) return std::unexpected(Error::TruncatedInput);
			if (type == 3) return std::unexpected(Error::BadData);
			if (!type) {
				// Discard padding before the byte-aligned stored block.
				reader.value = reader.bits = 0;
				const auto count{reader.read(16)};
				const auto inverse{reader.read(16)};
				if (reader.failed) return std::unexpected(Error::TruncatedInput);
				if ((count ^ inverse) != 65535) return std::unexpected(Error::BadData);
				if (count > input.size() - reader.position) return std::unexpected(Error::TruncatedInput);
				if (count > maxOutput - out.size()) return std::unexpected(Error::OutputLimit);

				const auto bytes{input.subspan(reader.position, count)};
				out.insert(out.end(), bytes.begin(), bytes.end());
				reader.position += count;
				continue;
			}

			Huffman literals{};
			Huffman distances{};
			if (type == 1) {
				fixedTrees(literals, distances);
			}
			else {
				const auto trees{dynamicTrees(reader, literals, distances)};
				if (!trees) return std::unexpected(trees.error());
			}

			while (true) {
				const auto symbol{decodeSymbol(literals, reader)};
				if (!symbol) return std::unexpected(symbol.error());
				if (*symbol == 256) break;
				if (*symbol < 256) {
					if (out.size() == maxOutput) return std::unexpected(Error::OutputLimit);
					out.push_back(as<u8>(*symbol));
					continue;
				}
				if (*symbol > 285) return std::unexpected(Error::BadData);

				const auto code{*symbol - 257};
				const auto length{LengthBase[code] + reader.read(LengthBits[code])};
				if (reader.failed) return std::unexpected(Error::TruncatedInput);

				const auto distanceCode{decodeSymbol(distances, reader)};
				if (!distanceCode) return std::unexpected(distanceCode.error());
				if (*distanceCode >= 30) return std::unexpected(Error::BadData);

				const auto distance{DistanceBase[*distanceCode] + reader.read(DistanceBits[*distanceCode])};
				if (reader.failed) return std::unexpected(Error::TruncatedInput);
				if (distance > out.size()) return std::unexpected(Error::BadData);
				if (length > maxOutput - out.size()) return std::unexpected(Error::OutputLimit);

				for ([[maybe_unused]] const auto i : were::thru(length)) {
					out.push_back(out[out.size() - distance]);
				}
			}
		}
		if (reader.position != input.size()) return std::unexpected(Error::TrailingData);

		return out;
	}
}

// [Static Assert Testing]
namespace were::deflate::testing {
	static_assert(LengthBase.front() == 3 && LengthBase.back() == 258);
	static_assert(LengthBits[27] == 5 && LengthBits.back() == 0);
	static_assert(DistanceBase.front() == 1 && DistanceBase.back() == 24577);
	static_assert(DistanceBits.back() == 13);

	static_assert([] {
		constexpr auto input = std::to_array<u8>({'a', 'b', 'c'});
		// Independently encoded raw DEFLATE: stored and fixed Huffman blocks.
		const auto stored = *were::hex::decode("010300FCFF616263");
		const auto fixed = *were::hex::decode("4B4C4A0600");
		const auto plain{std::vector<u8>(input.begin(), input.end())};

		return compress(input, 0) == stored
			&& decompress(stored) == plain && decompress(fixed) == plain;
	}(), "DEFLATE stored and fixed vectors failed");

	static_assert(decompress(std::to_array<u8>({3, 0}))->empty());
	static_assert(decompress(std::to_array<u8>({7})).error() == Error::BadData);
	static_assert(decompress(std::to_array<u8>({3})).error() == Error::TruncatedInput);
	static_assert(decompress(std::to_array<u8>({3, 0, 0})).error() == Error::TrailingData);
	static_assert(decompress(*were::hex::decode("4B4C4A0600"), 2).error() == Error::OutputLimit);
	static_assert(compress(std::span<const u8>{}, 10).error() == Error::BadLevel);
}
