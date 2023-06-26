#include "StdAfx.h"

#include "LogSystem.h"
#include <stdarg.h>

void LogSystem::LogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogInternal(format, args);
}

void LogSystem::LogWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogInternal(format, args);
}

void LogSystem::LogInfo(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogInternal(format, args);
}
void LogSystem::LogInternal(const char* format, va_list args)
{
	vsnprintf(m_buff, LOGSYSTEM_MAX_BUFFER - 1, format, args);
	// Make sure there's a limit to the amount of rubbish we can output
	m_buff[LOGSYSTEM_MAX_BUFFER - 1] = '\0';

	LOG_VERBOSE(m_buff);
}