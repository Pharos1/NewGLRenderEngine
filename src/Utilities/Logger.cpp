#include "../pch.h"
#include "Logger.hpp"

namespace Log{
	ExcludedLevels::ExcludedLevels(uint16_t flags)
		: flagValue(flags){
	}
	void ExcludedLevels::set(LogLevel flag) {
		flagValue |= (int)flag;
	}
	void ExcludedLevels::unset(LogLevel flag) {
		flagValue &= ~(int)flag;
	}
	void ExcludedLevels::flip(LogLevel flag) {
		flagValue ^= (int)flag;
	}
	bool ExcludedLevels::has(LogLevel flag) const {
		return (flagValue & (int)flag) == (int)flag;
	}

	ExcludedLevels excludedLevels(0);

	void log(const std::string& message, LogLevel type, int line, const char* fileName) {
		if (excludedLevels.has(type)) return;

		switch (type) {
		case LogInfo:
			std::cerr << "INFO";
			break;
		case LogDestructorInfo:
			std::cerr << "DESTRUCTOR_INFO";
			break;
		case LogWarning:
			std::cerr << "WARNING";
			break;
		case LogError:
			std::cerr << "ERROR";
			break;
		}

		std::cerr << ": " << message;
		if (fileName)
			std::cerr << " File '" << fileName << "'.";
		if(line > 0)
			std::cerr << " Line " << line << ".";
		std::cerr << std::endl;

		if (type & LogError)
			throw std::exception(); //exit(-1);
	}
}