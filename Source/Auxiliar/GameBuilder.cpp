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
		std::wstring buildScriptPath = L"..\\Source\\BuildScripts\\";
		std::wstring buildScript = buildScriptPath;
		switch (buildType)
		{
		case BuildType::DEBUG_GAME:
			ENGINE_LOG("Building DebugGame...\n");
			buildScript += L"buildDebug";
			break;
		case BuildType::RELEASE_GAME:
			ENGINE_LOG("Building ReleaseGame...\n");
			buildScript += L"buildRelease";
			break;
		}
		buildScript += L".bat";

		STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION processInfo = {};
		BOOL success = CreateProcess(
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

		const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

		std::vector<std::string> scenes = fileSystem->ListFiles(SCENE_PATH);
		for (const std::string& sceneFileName : scenes)
		{
			fileSystem->Copy(SCENE_PATH + sceneFileName, "Lib/Scenes/" + sceneFileName);
		}

		fileSystem->ZipLibFolder();
	}
}
