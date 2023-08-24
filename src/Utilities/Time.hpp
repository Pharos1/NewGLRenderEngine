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

	void start();
	void stop();
};

class ScopedTimer {
public:
	std::chrono::high_resolution_clock::time_point startTime;
	std::string name;

	ScopedTimer(const std::string& name);
	~ScopedTimer();

	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator=(const ScopedTimer&) = delete;
};