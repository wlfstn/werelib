#pragma once

#include <string_view>
#include <string>
#include <cstring>
#include <ranges>
#include <span>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <d3d11on12.h>

#include "../werelib.hpp"

namespace were::spout {
	constexpr std::wstring_view FXD_SPOUT_LIST =  L"Global\\SpoutSenderNames";
	constexpr std::wstring_view FXD_SPOUT_MUTEX =  L"Global\\SpoutSenderMutex";
	constexpr int FXD_SPOUT_MEM = 10240;
	constexpr int FXD_SENDER_SLOT = 256;
	constexpr int FXD_SENDERS = FXD_SPOUT_MEM / FXD_SENDER_SLOT;
	
	struct DX12Connection {
		ID3D12Device* device = nullptr;
		ID3D12CommandQueue* commandQueue = nullptr;
		ID3D12Resource* renderTarget = nullptr;
		std::wstring senderName;
		UINT width = 0;
		UINT height = 0;
	};

	#pragma pack(push, 1)
	struct SpoutHeader {
		u32 width = 0, height = 0;
		DWORD format = DXGI_FORMAT_UNKNOWN;
		HANDLE sharedHandle = nullptr;
		u32 frameNumber = 0;
		u32 isConnected = 0;
		u8 reserved[256] = {};
	};
	#pragma pack(pop)

	class SpoutOBJ {
	public:
		// -- CONSTRUCTOR / DESTRUCTOR --
		SpoutOBJ(std::wstring_view name);
	private:
		// -- INTERNAL FUNCTIONS -- 
		void CreateSpoutMemory();
		void CreateSpoutSender();
		void Mutex();
		void RegisterSender();

		// -- DATA MEMBERS -- 
		HANDLE hSpoutMap_ = nullptr;
		void* pSpoutMap_ = nullptr;
		
		HANDLE hSpoutMutex_ = nullptr;

		std::wstring senderName_ = L"wereSpout";
		HANDLE hSpoutSender_ = nullptr;
		void* pSpoutSender_ = nullptr;

		HANDLE hSenderList_ = nullptr;
		void* pSenderList_ = nullptr;
	};
}