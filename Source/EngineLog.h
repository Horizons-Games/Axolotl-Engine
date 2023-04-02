#pragma once

#include "Globals.h"

#include <memory>
#include <string>
#include <vector>

#ifndef ENGINE
#	define ENGINE_LOG(...) // ignore it
#else
#	define ENGINE_LOG(format, ...) engineLog->Log(__FILE__, __LINE__, format, __VA_ARGS__);

class EngineLog
{
public:
	EngineLog(){};

	~EngineLog();

	void Log(const char file[], int line, const char* format, ...);

public:
	std::vector<std::string> logLines;
};

extern std::unique_ptr<EngineLog> engineLog;

inline EngineLog::~EngineLog()
{
	logLines.clear();
}
#endif // ENGINE
