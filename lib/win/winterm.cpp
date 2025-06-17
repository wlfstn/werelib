#include "winterm.hpp"

namespace were::winterm {

	Console::Console() {
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void Console::SetPos(i16 x, i16 y) {
		COORD coord = { x, y };
		SetConsoleCursorPosition(hConsole, coord);
	}

	void Console::Clear() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
		DWORD written;
		FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, { 0,0 }, &written);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, { 0,0 }, &written);
		SetPos(0, 0);
	}

}