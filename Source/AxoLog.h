#pragma once

#include "Globals.h"

#include <vector>
#include <string>
#include <memory>

#ifndef ENGINE
#define ENGINE_LOG(...) //ignore it
#else
#define ENGINE_LOG(format, ...) logContext->Log(__FILE__, __LINE__, format, __VA_ARGS__);

class GameObject;

class EngineLog
{
public:
	EngineLog() = default;
	~EngineLog() = default;

	template<typename... Args>
	void Log(const char file[], int line, const std::string& format, Args&&... args);
	void StartWritingToFile();
	void StopWritingToFile();

private:
	void Write(const char file[], int line, const std::string& formattedLine);

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

	// this vector is cleared every time we log to the console
	std::vector<std::string> logLines;
	bool writingToFile = false;
};

template<typename... Args>
void EngineLog::Log(const char file[], int line, const std::string& format, Args&&... args)
{
	std::string formattedString = format;
	// this iterates through the variadic argument pack like a loop
	([&]()
		{
			if (!Format(formattedString, args))
			{
				Log(file, line, "Too many arguments in log call!");
			}
		}(),
	...);

	size_t firstToken = FindReplaceToken(formattedString);
	if (firstToken != std::string::npos)
	{
		Log(file, line, "Too few arguments in log call!");
	}

	Write(file, line, formattedString);
}

extern std::unique_ptr<EngineLog> logContext;
#endif // ENGINE
