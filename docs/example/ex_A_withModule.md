```cpp
#include <iostream>
#include <vector>
#include <iomanip>

import werelib;

int main() {
	constexpr double d = 27.14;
	int i = were::as<int>(d);
	std::array<were::byte, sizeof(i)> y = were::asBytes(i);
	int x = were::bigEndianSwap(i);

	std::cout << "Hello world\ni = " << i << "\n"
	<< "hex value: 0x" << std::hex << std::setw(8) << std::setfill('0') << i <<"\n"
	<< "hex value Endian Swap: 0x" << x << std::endl;
}
```