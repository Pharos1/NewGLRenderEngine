#pragma once
#include "../pch.h"

namespace Time {
	extern float deltaTime;
	extern std::chrono::high_resolution_clock::time_point lastDeltaTime;

	extern float deltaSum;
	extern std::chrono::high_resolution_clock::time_point lastFPSCount;

	extern double avgMsTime;
	extern uint32_t frameCount;

	void updateDelta();
}
class Timer {
public:
	std::chrono::high_resolution_clock::time_point startTime;
	std::string name;

	Timer(const std::string& name);
	Timer() = default;
	~Timer();

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
};