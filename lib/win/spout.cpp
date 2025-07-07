#include "spout.hpp"

namespace were::spout {

	SpoutOBJ::SpoutOBJ(std::wstring name) {
		CreateSpoutMemory();
		senderName_ = name;
		CreateSpoutSender();
		Mutex();
	}

	void SpoutOBJ::CreateSpoutMemory() {
		hSpoutMap_ = CreateFileMappingW(
			INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
			0, 10240,
			FXD_SPOUT_LIST.data()
		);

		pSpoutMap_ = MapViewOfFile(hSpoutMap_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}

	void SpoutOBJ::CreateSpoutSender() {
		hSpoutSender_ = CreateFileMappingW(
			INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
			0, 512,
			(L"Global\\" + senderName_).c_str()
		);

		pSpoutSender_ = MapViewOfFile(hSpoutSender_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}

	void SpoutOBJ::Mutex() {
		hSpoutMutex_ = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"Global\\SpoutSenderMutex");
		WaitForSingleObject(hSpoutMutex_, INFINITE);
	}

	void SpoutOBJ::RegisterSender() {
		hSenderList_ = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, FXD_SPOUT_LIST.data());
		pSenderList_ = MapViewOfFile(hSenderList_, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		std::span<byte> senderList(as<byte*>(pSenderList_), FXD_SPOUT_MEM);
		auto slot_view = senderList | std::views::chunk(FXD_SENDER_SLOT);
		std::string SenderNameNarrow(senderName_.begin(), senderName_.end());
		for (auto [index, slot] : slot_view | std::views::enumerate) {
			if (slot[0] == 0x00) {
				std::memcpy(slot.data(), SenderNameNarrow.c_str(), std::min<size_t>(255, SenderNameNarrow.size()));
				slot[SenderNameNarrow.size()] = 0x00;
				break;
			}
		}
	}

}