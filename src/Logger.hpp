#pragma once
#include <iostream>

#define mLog(x, y) Log::log(x, y, __LINE__);

namespace Log {
	enum LogLevel : uint32_t {
		LogInfo,
		LogWarning,
		LogError,
	};
	extern LogLevel minLevel;
	void log(const std::string& message, LogLevel type = LogError, int line = 0);
};