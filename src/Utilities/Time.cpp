#include "../pch.h"
#include "Time.hpp"

namespace Time {
	float deltaTime = 0;
	std::chrono::high_resolution_clock::time_point lastDelta = std::chrono::high_resolution_clock::now();

	float deltaSum = 0;
	std::chrono::high_resolution_clock::time_point lastFPSCount = std::chrono::high_resolution_clock::now();

	double avgMsTime = 0;
	uint32_t frameCount = 0;

	void update() {
		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(now - lastDelta).count();

		lastDelta = now;
		float fromLastCount = std::chrono::duration<float, std::chrono::seconds::period>(now - lastFPSCount).count();

		if (fromLastCount >= 1.f && frameCount != 0) {
			lastFPSCount = now;
			avgMsTime = deltaSum / frameCount;
			avgMsTime *= 1000;

			deltaSum = 0;
			frameCount = 0;
		}
		else {
			deltaSum += deltaTime;
			frameCount += 1;
		}
	}
}