
#pragma once

#ifndef LOGSYSTEM_INCLUDED
	#define LOGSYSTEM_INCLUDED

	#include "ICompilerLogger.h"

	#include <stdio.h>

// StdioLogSystem for compiler

const size_t LOGSYSTEM_MAX_BUFFER = 4096;

class LogSystem : public ICompilerLogger
{
public:
	virtual void LogError(const char* format, ...);
	virtual void LogWarning(const char* format, ...);
	virtual void LogInfo(const char* format, ...);

protected:
	void LogInternal(const char* format, va_list args);
	char m_buff[LOGSYSTEM_MAX_BUFFER];
};
#endif // STDIOLOGSYSTEM_INCLUDED