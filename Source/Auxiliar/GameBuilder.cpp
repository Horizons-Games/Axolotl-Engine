#include "Auxiliar/GameBuilder.h"

#include "EngineLog.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include <string>
#include <assert.h>
#include <windows.h>
#include <future>

namespace builder
{
	namespace
	{
		std::future<void> compileThread;
		std::future<void> zipThread;

		void CompileGame(const std::wstring& batchFilePath)
		{
			STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION processInfo = {};
			BOOL success = CreateProcess(
				batchFilePath.c_str(), // lpApplicationName
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

			ENGINE_LOG("Done compiling!");
		}

		void CreateZip()
		{
			const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

			std::vector<std::string> scenes = fileSystem->ListFiles(SCENE_PATH);
			for (const std::string& sceneFileName : scenes)
			{
				fileSystem->Copy(SCENE_PATH + sceneFileName, "Lib/Scenes/" + sceneFileName);
			}

			fileSystem->ZipLibFolder();

			ENGINE_LOG("Done creating ZIP!");
		}
	}
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

		compileThread = std::async(std::launch::async, &CompileGame, buildScript);
		zipThread = std::async(std::launch::async, &CreateZip);
	}

	bool Compiling()
{
		return compileThread.valid() && !compileThread._Is_ready();
	}

	bool Zipping()
{
		return zipThread.valid() && !zipThread._Is_ready();
	}

}
