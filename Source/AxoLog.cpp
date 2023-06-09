#include "AxoLog.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/GameObject/GameObject.h"
#include "DataModels/Resources/Resource.h"

#include <assert.h>
#include <mutex>

namespace
{
const char* documentsPath = "Documents";
const char* logFilePath = "Documents/Axolotl.log";

const char* replaceToken = "{}";

std::recursive_mutex writeLock;
} // namespace

void AxoLog::Write(const char file[], int line, LogSeverity severity, const std::string& formattedLine)
{
	// Lock the mutex so the function is thread-safe
	// this scoped_lock will be destroyed upon exiting the method, thus freeing the mutex
	// according to the documentation, this is deadlock-safe,
	// meaning that there won't be any deadlocks if the mutex is attempted to be locked twice
	std::scoped_lock lock(writeLock);

	LogLine logLine{ severity, file, line, formattedLine };
	logLines.push_back(logLine);

	std::string detailedString = logLine.ToDetailedString();

	OutputDebugStringA(detailedString.c_str());

	bool recursiveLogCall = file == __FILE__;
	if (writingToFile && !recursiveLogCall)
	{
		if (App == nullptr || App->GetModule<ModuleFileSystem>() == nullptr)
		{
			LOG_ERROR("Error writing to log file, FileSystem already terminated");
			return;
		}
		if (App->GetModule<ModuleFileSystem>()->Save(
				logFilePath, detailedString.c_str(), detailedString.size(), true) == 1)
		{
			LOG_ERROR("Error writing to log file");
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
	return formatString.find(replaceToken);
}

bool AxoLog::Format(std::string& format, int arg) const
{
	return Format(format, std::to_string(arg));
}

bool AxoLog::Format(std::string& format, unsigned int arg) const
{
	return Format(format, std::to_string(arg));
}

bool AxoLog::Format(std::string& format, float arg) const
{
	return Format(format, std::to_string(arg));
}

bool AxoLog::Format(std::string& format, const char* arg) const
{
	return Format(format, std::string(arg));
}

bool AxoLog::Format(std::string& format, const std::string& arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, strlen(replaceToken), arg);
		return true;
	}
	return false;
}

bool AxoLog::Format(std::string& format, bool arg) const
{
	return Format(format, std::to_string(arg));
}

bool AxoLog::Format(std::string& format, const GameObject* arg) const
{
	return Format(format, arg->GetName());
}

bool AxoLog::Format(std::string& format, unsigned long long arg) const
{
	return Format(format, std::to_string(arg));
}

bool AxoLog::Format(std::string& format, const Resource* arg) const
{
	return Format(format, arg->GetUID());
}

bool AxoLog::Format(std::string& format, const unsigned char* arg) const
{
	return Format(format, reinterpret_cast<const char*>(arg));
}
