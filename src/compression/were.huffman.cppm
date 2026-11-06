module;

#include <array>
#include <cstddef>
#include <expected>
#include <ranges>
#include <queue>
#include <span>
#include <utility>
#include <vector>

export module were.huffman;
import weretype;

export namespace were::huffman {

	enum struct Error { EmptyTree, BadLength, Oversubscribed, BadSymbol, BadCode, CountOverflow, TruncatedInput };

	// Canonical codes ordered by length, then symbol. Zero lengths omit symbols.
	template<std::size_t SymbolCount, u8 MaxBits = 15>
	class CanonicalTree {
		static_assert(SymbolCount > 0 && SymbolCount <= were::numeric<u32>::max);
		static_assert(MaxBits > 0 && MaxBits < were::memsize<u32>::bits);
		std::array<u32, MaxBits + 1> counts{};
		std::array<u32, SymbolCount> symbols{};
		u32 size{};

	public:
		[[nodiscard]] constexpr
		auto build(std::span<const u8> lengths, bool complete = false, bool allowEmpty = false)
			-> std::expected<void, Error> {
			// A failed rebuild leaves no usable codes.
			size = 0;
			counts.fill(0);
			if (lengths.size() > SymbolCount) return std::unexpected(Error::BadSymbol);
			for (const auto length : lengths) {
				if (length > MaxBits) return std::unexpected(Error::BadLength);
				if (length) ++counts[length];
			}
			i64 available = 1;
			u32 used = 0;
			for (const auto bits : were::thru(MaxBits)) {
				available = available * 2 - counts[bits + 1];
				if (available < 0) return std::unexpected(Error::Oversubscribed);
				used += counts[bits + 1];
			}
			if (!used) {
				if (allowEmpty) return {};
				return std::unexpected(Error::EmptyTree);
			}
			if (available && (complete || used != 1 || counts[1] != 1))
				return std::unexpected(Error::BadLength);

			std::array<u32, MaxBits + 1> offsets{};
			for (const auto bits : were::thru(MaxBits - 1))
				offsets[bits + 2] = offsets[bits + 1] + counts[bits + 1];
			for (const auto [symbol, length] : were::thru(lengths)) {
				if (length) symbols[offsets[length]++] = as<u32>(symbol);
			}
			size = used;
			return {};
		}

		// readBit returns expected<u8, Error>; code bits arrive most significant first.
		template<class ReadBit>
		[[nodiscard]] constexpr
		auto decode(ReadBit&& readBit) const -> std::expected<u32, Error> {
			if (!size) return std::unexpected(Error::EmptyTree);
			u32 code = 0;
			u32 first = 0;
			u32 index = 0;
			for (const auto bits : were::thru(MaxBits)) {
				const auto bit = readBit();
				if (!bit) return std::unexpected(bit.error());
				if (*bit > 1) return std::unexpected(Error::BadCode);
				code = (code << 1) | *bit;
				const auto count = counts[bits + 1];
				if (code >= first && code - first < count) return symbols[index + code - first];
				index += count;
				first = (first + count) << 1;
			}
			return std::unexpected(Error::BadCode);
		}
	};

	struct ByteCount {
		u8 byteval;
		std::size_t count;
	};

	inline constexpr auto NoNode = were::numeric<std::size_t>::max;

	struct Node {
		u8 byteval{}; // Meaningful only for leaves.
		std::size_t count{};
		std::size_t left = NoNode;
		std::size_t right = NoNode;

		[[nodiscard]] constexpr
		auto isLeaf() const -> bool { return left == NoNode && right == NoNode; }
	};

	struct Tree {
		std::vector<Node> nodes;
		std::size_t root = NoNode;
	};

	// One entry per distinct byte, ordered by byte value.
	[[nodiscard]] constexpr
	auto countBytes(std::span<const u8> input) -> std::vector<ByteCount> {

		std::array<std::size_t, were::numeric<u8>::cardinality()> quantities{};
		for (auto value : input) ++quantities[value];

		std::vector<ByteCount> out;
		for (auto [value, qty] : were::thru(quantities)) {
			if (qty == 0) continue;
			out.push_back({as<u8>(value), qty});
		}
		return out;
	}

	// Leaves are ordered by byte value; ties are resolved by node index.
	[[nodiscard]] constexpr
	auto buildTree(std::span<const ByteCount> counts) -> std::expected<Tree, Error> {
		std::array<std::size_t, were::numeric<u8>::cardinality()> frequencies{};
		std::array<bool, were::numeric<u8>::cardinality()> seen{};
		std::size_t total = 0;
		for (auto [byteval, count] : counts) {
			if (seen[byteval]) return std::unexpected(Error::BadSymbol);
			seen[byteval] = true;
			if (count > were::numeric<std::size_t>::max - total)
				return std::unexpected(Error::CountOverflow);
			total += count;
			frequencies[byteval] = count;
		}
		if (total == 0) return std::unexpected(Error::EmptyTree);

		Tree tree;
		const auto greater = [&tree](std::size_t lhs, std::size_t rhs) -> bool {
			return std::pair{tree.nodes[lhs].count, lhs} > std::pair{tree.nodes[rhs].count, rhs};
		};
		std::priority_queue active(greater, std::vector<std::size_t>{});
		const auto addNode = [&](Node node) -> void {
			tree.nodes.push_back(node);
			active.push(tree.nodes.size() - 1);
		};
		const auto popSmallest = [&]() -> std::size_t {
			const auto index = active.top();
			active.pop();
			return index;
		};

		for (auto [byteval, count] : were::thru(frequencies)) {
			if (count == 0) continue;
			addNode({.byteval = as<u8>(byteval), .count = count});
		}
		tree.nodes.reserve(2 * active.size() - 1);

		while (active.size() > 1) {
			const auto left = popSmallest();
			const auto right = popSmallest();
			addNode({
				.count = tree.nodes[left].count + tree.nodes[right].count,
				.left = left,
				.right = right,
			});
		}
		tree.root = active.top();
		return tree;
	}

	[[nodiscard]] constexpr
	auto buildTree(std::span<const u8> input) -> std::expected<Tree, Error> {
		auto counts = countBytes(input);
		return buildTree(counts);
	}
}
