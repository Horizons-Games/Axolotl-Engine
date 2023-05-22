#pragma once

#include "Globals.h"

#include <memory>
#include <string>
#include <vector>

#ifndef ENGINE
	#define LOG_INFO(...)	 // ignore it
	#define LOG_VERBOSE(...) // ignore it
	#define LOG_WARNING(...) // ignore it
	#define LOG_ERROR(...)	 // ignore it
#else
enum class LogSeverity
{
	INFO,
	VERBOSE,
	WARNING,
	ERROR_ // "ERROR" is a protected definition
};
	#define LOG_INFO(format, ...) logContext->Log(__FILE__, __LINE__, LogSeverity::INFO, format, __VA_ARGS__);
	#define LOG_VERBOSE(format, ...) logContext->Log(__FILE__, __LINE__, LogSeverity::VERBOSE, format, __VA_ARGS__);
	#define LOG_WARNING(format, ...) logContext->Log(__FILE__, __LINE__, LogSeverity::WARNING, format, __VA_ARGS__);
	#define LOG_ERROR(format, ...) logContext->Log(__FILE__, __LINE__, LogSeverity::ERROR_, format, __VA_ARGS__);

class GameObject;

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
	void Write(const char file[], int line, LogSeverity severity, const std::string& formattedLine);

	size_t FindReplaceToken(const std::string& formatString) const;

	bool Format(std::string& format, int arg) const;
	bool Format(std::string& format, unsigned int arg) const;
	bool Format(std::string& format, float arg) const;
	bool Format(std::string& format, const char* arg) const;
	bool Format(std::string& format, const std::string& arg) const;
	bool Format(std::string& format, bool arg) const;
	bool Format(std::string& format, const GameObject* arg) const;
	// for glew logging
	bool Format(std::string& format, const unsigned char* arg) const;

private:
	friend class WindowConsole;

	struct LogLine
	{
		LogSeverity severity;
		std::string file;
		unsigned int line;
		std::string message;

		std::string ToDetailedString(bool addBreak = true) const
		{
			return ToString(true, addBreak);
		}

		std::string ToSimpleString(bool addBreak = true) const
		{
			return ToString(false, addBreak);
		}

	private:
		std::string ToString(bool detailed, bool addBreak) const
		{
			std::string result;
			switch (severity)
			{
				case LogSeverity::INFO:
					result = "[INFO]";
					break;
				case LogSeverity::VERBOSE:
					result = "[VERBOSE]";
					break;
				case LogSeverity::WARNING:
					result = "[WARNING]";
					break;
				case LogSeverity::ERROR_:
					result = "[ERROR]";
					break;
			}
			if (detailed)
			{
				result += file + "(" + std::to_string(line) + ") : ";
			}
			result += message;
			if (addBreak)
			{
				result += '\n';
			}
			return result;
		}
	};

	// this vector is cleared every time we log to the console
	std::vector<LogLine> logLines;
	bool writingToFile = false;
};

template<typename... Args>
void AxoLog::Log(const char file[], int line, LogSeverity severity, const std::string& format, Args&&... args)
{
	std::string formattedString = format;
	// this iterates through the variadic argument pack like a loop
	(
		[&]()
		{
			if (!Format(formattedString, args))
			{
				Log(file, line, LogSeverity::WARNING, "Too many arguments in log call!");
			}
		}(),
		...);

	size_t firstToken = FindReplaceToken(formattedString);
	if (firstToken != std::string::npos)
	{
		Log(file, line, LogSeverity::WARNING, "Too few arguments in log call!");
	}

	Write(file, line, severity, formattedString);
}

extern std::unique_ptr<AxoLog> logContext;
#endif // ENGINE
