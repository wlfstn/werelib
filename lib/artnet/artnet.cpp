#include "artnet.hpp"

namespace were::artnet {
		
	bool DMX_Artnet::HeaderValid() const {
		return packet.signature == FXD_SIGNATURE;
	}

	bool DMX_Artnet::ProcPacket(std::span<const u8> buffer) {
		if (buffer.size() < MIN_PACKET_SIZE || buffer.size() > MAX_PACKET_SIZE) {
			std::cerr << "[ArtNet] Packet size invalid: " << buffer.size() << "\n";
			return false;
		}

		ArtDMX temp{};
		std::memcpy(&temp, buffer.data(), std::min(buffer.size(), sizeof(ArtDMX)));

		if (temp.signature != FXD_SIGNATURE) {
			std::cerr << "[ArtNet] Invalid signature in packet\n";
			return false;
		}

		if (temp.operation != Op::Dmx) {
			std::cerr << "[ArtNet] Unsupported OpCode: " << std::hex << static_cast<u16>(temp.operation) << "\n";
			return false;
		}

		if (temp.dmxLength > 512 || (temp.dmxLength % 2) != 0) {
			std::cerr << "[ArtNet] Invalid DMX length: " << temp.dmxLength << "\n";
			return false;
		}

		packet = temp;
		return true;
	}

	std::span<const u8> DMX_Artnet::DMX_view() const {
		return std::span<const u8>(packet.dmxData.data(), packet.dmxLength);
	}

}