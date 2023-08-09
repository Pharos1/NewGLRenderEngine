#pragma once
#include <iostream>

#define mLog(message, type, location) Log::log(message, type, location, __LINE__, __FILE__);
#define nLog(message, type, location) Log::log(message, type, location);

namespace Log {
	enum LogLevel : uint32_t {
		LogInfo				= 1 << 0,
		LogDestructorInfo	= 1 << 1,
		LogWarning			= 1 << 2,
		LogError			= 1 << 3,
	};
    class ExcludedLevels {
    public:
        ExcludedLevels(uint16_t flags);
        ExcludedLevels() = default;

        uint16_t flagValue = 0;

        void set(LogLevel flag);
        void unset(LogLevel flag);
        void flip(LogLevel flag);
        bool has(LogLevel flag) const;
    };

	void log(const std::string& message, LogLevel type = LogError, const std::string& location = "", int line = 0, const char* fileName = nullptr);
};