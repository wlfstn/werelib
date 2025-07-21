#include "werelib.hpp"

namespace were {	
	struct LifeTimer {
		std::chrono::time_point<std::chrono::steady_clock> start, end;
		std::chrono::duration<f32> duration;
		
		LifeTimer() {
			start = std::chrono::steady_clock::now();
		}
		
		~LifeTimer() {
			end = std::chrono::steady_clock::now();
			duration = end - start;
			
			float ms = duration.count() * 1000.0f;
			std::cout << "Timer length " << ms << "ms" << std::endl;
		}
	};
}