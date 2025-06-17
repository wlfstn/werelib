#pragma once

#include <windows.h>

#include "../werelib.hpp"

// Windows terminal or console
namespace were::winterm {

	class Console {
		private:
			HANDLE hConsole;

		public:
			Console();
			void SetPos(i16 x, i16 y);
			void Clear();

	};
}