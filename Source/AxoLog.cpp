#include "AxoLog.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/GameObject/GameObject.h"
#include "DataModels/Resources/Resource.h"

#include <assert.h>

const char* documentsPath = "Documents";
const char* logFilePath = "Documents/Axolotl.log";

void AxoLog::Write(const char file[], int line, LogSeverity severity, const std::string& formattedLine)
{
	LogLine logLine{ severity, file, line, formattedLine };
	logLines.push_back(logLine);

	std::string detailedString = logLine.ToDetailedString();

	OutputDebugStringA(detailedString.c_str());

	if (writingToFile)
	{
		// set it to false so, if there's an error saving, we don't cause a stack overflow
		writingToFile = false;
		if (App == nullptr || App->GetModule<ModuleFileSystem>() == nullptr)
		{
			LOG_ERROR("Error writing to log file, FileSystem already terminated");
			return;
		}
		if (App->GetModule<ModuleFileSystem>()->Save(
				logFilePath, detailedString.c_str(), detailedString.size(), true) == 1)
		{
			LOG_ERROR("Error writing to log file, abort writing for the rest of the execution");
		}
		else
		{
			writingToFile = true;
		}
	}
}

void AxoLog::StartWritingToFile()
{
	assert(App);
	const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	assert(fileSystem);
	if (!fileSystem->Exists(documentsPath))
	{
		fileSystem->CreateDirectory(documentsPath);
	}
	// if folder does not exist, we know for sure the file won't either
	else if (fileSystem->Exists(logFilePath))
	{
		if (!fileSystem->Delete(logFilePath))
		{
			LOG_ERROR("FileSystem error; no logging will be saved this execution");
			return;
		}
	}
	for (std::string line : logLines | std::views::transform(
										   [](const LogLine& logLine)
										   {
											   return logLine.ToDetailedString();
										   }))
	{
		if (App->GetModule<ModuleFileSystem>()->Save(logFilePath, line.c_str(), line.size(), true) == 1)
		{
			LOG_ERROR("FileSystem error; no logging will be saved this execution");
			return;
		}
	}
	writingToFile = true;
}

void AxoLog::StopWritingToFile()
{
	LOG_INFO("Closing writer...");
	writingToFile = false;
}

size_t AxoLog::FindReplaceToken(const std::string& formatString) const
{
	return formatString.find("{}");
}

bool AxoLog::Format(std::string& format, int arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, unsigned int arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, float arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, const char* arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, arg);
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, const std::string& arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, arg);
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, bool arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, const GameObject* arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, arg->GetName());
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, unsigned long long arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, const Resource* arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg->GetUID()));
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, const unsigned char* arg) const
{
	return Format(format, reinterpret_cast<const char*>(arg));
}
