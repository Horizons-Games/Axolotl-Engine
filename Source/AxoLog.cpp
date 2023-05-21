#include "AxoLog.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/GameObject/GameObject.h"

#include <assert.h>

const char* logFilePath = "Axolotl.log";

void EngineLog::Write(const char file[], int line, const std::string& formattedLine)
{
	logLines.push_back(formattedLine);

	char lineWithFile[4096];
	sprintf_s(lineWithFile, 4096, "\n%s(%d) : %s", file, line, formattedLine.c_str());
	OutputDebugStringA(lineWithFile);

	if (writingToFile)
	{
		// set it to false so, if there's an error saving, we don't cause a stack overflow
		writingToFile = false;
		if (App == nullptr || App->GetModule<ModuleFileSystem>() == nullptr)
		{
			ENGINE_LOG("Error writing to log file, FileSystem already terminated");
			return;
		}
		if (App->GetModule<ModuleFileSystem>()->Save(logFilePath, lineWithFile, 4096, true) == 1)
		{
			ENGINE_LOG("Error writing to log file, abort writing for the rest of the execution");
		}
		else
		{
			writingToFile = true;
		}
	}
}

void EngineLog::StartWritingToFile()
{
	assert(App);
	const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	assert(fileSystem);
	if (fileSystem->Exists(logFilePath))
	{
		if (!fileSystem->Delete(logFilePath))
		{
			ENGINE_LOG("FileSystem error; no logging will be saved this execution");
			return;
		}
	}
	writingToFile = true;
}

void EngineLog::StopWritingToFile()
{
	ENGINE_LOG("Closing writer...");
	writingToFile = false;
}

size_t EngineLog::FindReplaceToken(const std::string& formatString) const
{
	return formatString.find("{}");
}

bool EngineLog::Format(std::string& format, int arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	} 
	return false;
}

bool EngineLog::Format(std::string& format, unsigned int arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool EngineLog::Format(std::string& format, float arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}

bool EngineLog::Format(std::string& format, const char* arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, arg);
		return true;
	}
	return false;
}

bool EngineLog::Format(std::string& format, const std::string& arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, arg);
		return true;
	}
	return false;
}

bool EngineLog::Format(std::string& format, bool arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, std::to_string(arg));
		return true;
	}
	return false;
}


bool EngineLog::Format(std::string& format, const GameObject* arg) const
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, 2, arg->GetName());
		return true;
	}
	return false;
}

bool EngineLog::Format(std::string& format, const unsigned char* arg) const
{
	return Format(format, reinterpret_cast<const char*>(arg));
}
