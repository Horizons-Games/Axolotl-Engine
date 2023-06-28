#pragma once

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

class GameObject;
class Resource;

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
	bool Format(std::string& format, unsigned long long arg) const;
	bool Format(std::string& format, const Resource* arg) const;
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
	std::string formattedString = format;
	// this iterates through the variadic argument pack like a loop
	(
		[&]()
		{
			if (!Format(formattedString, args))
			{
				Log(file, line, LogSeverity::WARNING_LOG, "Too many arguments in log call!");
			}
		}(),
		...);

	size_t firstToken = FindReplaceToken(formattedString);
	if (firstToken != std::string::npos)
	{
		Log(file, line, LogSeverity::WARNING_LOG, "Too few arguments in log call!");
	}

	Write(file, line, severity, formattedString);
}

extern std::unique_ptr<AxoLog> logContext;
#endif // ENGINE
