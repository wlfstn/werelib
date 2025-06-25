#pragma once

#include <windows.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <dxgi1_4.h>

namespace were::spout {

	struct SharedTextureDX12 {
		ID3D12Device* d3d12Device = nullptr;
		ID3D11Device* d3d11Device = nullptr;
		ID3D11DeviceContext* d3d11Context = nullptr;
		ID3D11On12Device* d3d11on12 = nullptr;
		ID3D11Texture2D* d3d11Texture = nullptr;
		IDXGIKeyedMutex* keyedMutex = nullptr;
		HANDLE sharedHandle = nullptr;
		UINT width = 0, height = 0;

		~SharedTextureDX12() {
			if (keyedMutex) keyedMutex->Release();
			if (d3d11Texture) d3d11Texture->Release();
			if (d3d11on12) d3d11on12->Release();
			if (d3d11Context) d3d11Context->Release();
			if (d3d11Device) d3d11Device->Release();
		}
	};

}