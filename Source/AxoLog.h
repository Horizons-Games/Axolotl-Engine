#pragma once
#include "Formatter.h"

#ifndef ENGINE
	#define LOG_INFO(...)	 // ignore it
	#define LOG_VERBOSE(...) // ignore it
	#define LOG_DEBUG(...)	 // ignore it
	#define LOG_WARNING(...) // ignore it
	#define LOG_ERROR(...)	 // ignore it
	#define AXO_LOG(...)	 // ignore it
#else
enum class LogSeverity
{
	INFO_LOG,
	VERBOSE_LOG,
	DEBUG_LOG,
	WARNING_LOG,
	ERROR_LOG
};
	#define LOG_INFO(format, ...) AXO_LOG(format, LogSeverity::INFO_LOG, __VA_ARGS__)
	#define LOG_VERBOSE(format, ...) AXO_LOG(format, LogSeverity::VERBOSE_LOG, __VA_ARGS__)
	#define LOG_DEBUG(format, ...) AXO_LOG(format, LogSeverity::DEBUG_LOG, __VA_ARGS__)
	#define LOG_WARNING(format, ...) AXO_LOG(format, LogSeverity::WARNING_LOG, __VA_ARGS__)
	#define LOG_ERROR(format, ...) AXO_LOG(format, LogSeverity::ERROR_LOG, __VA_ARGS__)
	#define AXO_LOG(format, severity, ...) logContext->Log(__FILE__, __LINE__, severity, format, __VA_ARGS__)

class AxoLog
{
public:
	AxoLog() = default;
	~AxoLog() = default;

	template<typename... Args>
	void Log(const char file[], int line, LogSeverity severity, const std::string& format, Args&&... args);
	void StartWritingToFile();
	void StopWritingToFile();

private:
	void Write(const char file[], int line, LogSeverity severity, std::string&& formattedLine);

private:
	friend class WindowConsole;

	struct LogLine
	{
		LogSeverity severity;
		std::string file;
		uint16_t line;
		std::string message;

		std::string ToDetailedString(bool addBreak = true) const;
		std::string ToSimpleString(bool addBreak = true) const;

	private:
		std::string ToString(bool detailed, bool addBreak) const;
	};

	// this vector is cleared every time we log to the console
	std::vector<LogLine> logLines;
	bool writingToFile = false;
};

template<typename... Args>
void AxoLog::Log(const char file[], int line, LogSeverity severity, const std::string& format, Args&&... args)
{
	Write(file, line, severity, axo::Format(format, std::forward<Args>(args)...));
}

extern std::unique_ptr<AxoLog> logContext;
#endif // ENGINE
