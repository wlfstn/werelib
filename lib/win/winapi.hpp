#include <windows.h>

namespace were::winapi {
	inline HANDLE SharedBlock() {
		return CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 256, L"MySharedMem");
	}
}
