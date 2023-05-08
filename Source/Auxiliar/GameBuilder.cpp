#include "Auxiliar/GameBuilder.h"

#include "EngineLog.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include <string>
#include <assert.h>
#include <windows.h>

namespace builder
{
	void BuildGame(BuildType buildType)
	{
		std::string buildScriptPath = "..\\Source\\BuildScripts\\";
		std::string buildScript = buildScriptPath;
		switch (buildType)
		{
		case BuildType::DEBUG_GAME:
			ENGINE_LOG("Building DebugGame...\n");
			buildScript += "buildDebug";
			break;
		case BuildType::RELEASE_GAME:
			ENGINE_LOG("Building ReleaseGame...\n");
			buildScript += "buildRelease";
			break;
		}
		buildScript += ".bat";

		STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION processInfo = {};

		BOOL success = CreateProcessA(
			buildScript.c_str(), // lpApplicationName
			NULL, // lpCommandLine
			NULL, // lpProcessAttributes
			NULL, // lpThreadAttributes
			TRUE, // bInheritHandles
			CREATE_NO_WINDOW, // dwCreationFlags
			NULL, // lpEnvironment
			NULL, // lpCurrentDirectory
			&startupInfo,
			&processInfo);
		assert(success);
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
	}
}
