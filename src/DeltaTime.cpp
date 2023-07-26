#include "pch.h"
#include "DeltaTime.hpp"

namespace DT {
	float time;
	std::chrono::high_resolution_clock::time_point lastTime;

	float deltaSum;
	std::chrono::high_resolution_clock::time_point lastFPSCount;

	double avgMsTime;
	uint32_t frameCount;

	void update() {
		auto now = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration<float, std::chrono::seconds::period>(now - lastTime).count();

		lastTime = now;
		float fromLastCount = std::chrono::duration<float, std::chrono::seconds::period>(now - lastFPSCount).count();

		if (fromLastCount >= 1.f) {
			lastFPSCount = now;
			avgMsTime = deltaSum / frameCount;

			deltaSum = 0;
			frameCount = 0;
		}
		else {
			deltaSum += time;
			frameCount += 1;
		}
	}
}