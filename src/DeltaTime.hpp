#pragma once
#include "pch.h"

namespace DT {
	extern float time;
	extern std::chrono::high_resolution_clock::time_point lastTime;

	extern float deltaSum;
	extern std::chrono::high_resolution_clock::time_point lastFPSCount;

	extern double avgMsTime;
	extern uint32_t frameCount;

	void update();
}