#include "pch.h"
#include "Logger.hpp"

namespace Log{
	LogLevel minLevel = LogInfo;
	void log(const std::string& message, LogLevel type, int line) {
		if (type < minLevel) return;

		switch (type) {
		case LogInfo:
			std::cout << "INFO";
			break;
		case LogWarning:
			std::cout << "WARNING";
			break;
		case LogError:
			std::cout << "ERROR";
			break;
		}

		std::cout << ((line > 0) ? (" at line " + std::to_string(line)) : "") << ": " << message << std::endl;

		if (type == LogError)
			exit(-1);
	}
}