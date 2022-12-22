#pragma once

#include "Globals.h"

#include <vector>
#include <string>
#include <memory>

#define ENGINE_LOG(format, ...) engineLog->Log(__FILE__, __LINE__, format, __VA_ARGS__);

class EngineLog
{
public:

	EngineLog() {};

	~EngineLog() 
	{ 
		logLines.clear();
	}

	void Log(const char file[], int line, const char* format, ...);

public:
	std::vector<std::string> logLines;
};

extern std::unique_ptr<EngineLog> engineLog;