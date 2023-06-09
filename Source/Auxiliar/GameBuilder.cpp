#include "Auxiliar/GameBuilder.h"

#include "Application.h"
#include "AxoLog.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"

#include <assert.h>
#include <future>
#include <string>
#include <windows.h>

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
	BOOL success = CreateProcess(batchFilePath.c_str(), // lpApplicationName
								 NULL,					// lpCommandLine
								 NULL,					// lpProcessAttributes
								 NULL,					// lpThreadAttributes
								 TRUE,					// bInheritHandles
								 CREATE_NO_WINDOW,		// dwCreationFlags
								 NULL,					// lpEnvironment
								 NULL,					// lpCurrentDirectory
								 &startupInfo,
								 &processInfo);
	assert(success);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	LOG_INFO("Done compiling!");
}

void CopyFolderInLib(const std::string& sourcePath, const std::string& destinationInsideLib)
{
	const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	std::string fullLibPath = "Lib/" + destinationInsideLib;
	if (!fileSystem->Exists(fullLibPath.c_str()))
	{
		fileSystem->CreateDirectory(fullLibPath.c_str());
	}

	std::vector<std::string> items = fileSystem->ListFiles(sourcePath.c_str());
	for (const std::string& itemName : items)
	{
		std::string itemAndPath = sourcePath + itemName;
		if (fileSystem->IsDirectory(itemAndPath.c_str()))
		{
			CopyFolderInLib(itemAndPath + "/", destinationInsideLib + itemName + "/");
		}
		else
		{
			fileSystem->Copy(itemAndPath, fullLibPath + itemName);
		}
	}
}

void AddConfigToZip(const std::string& startingScene)
{
	std::string zipPath = "Assets.zip";
	assert(App->GetModule<ModuleFileSystem>()->Exists(zipPath.c_str()));

	rapidjson::Document doc;
	Json startConfig(doc, doc);

	startConfig["StartingScene"] = startingScene.c_str();

	rapidjson::StringBuffer buffer;
	startConfig.toBuffer(buffer);

	std::string path = GAME_STARTING_CONFIG;

	App->GetModule<ModuleFileSystem>()->AppendToZipFolder(zipPath, path.c_str(), buffer.GetString(), buffer.GetSize(), true);
}

void CreateZip(const std::string& startingScene)
{
	CopyFolderInLib(SCENE_PATH, "Scenes/");
	CopyFolderInLib("Source/Shaders/", "Shaders/");

	App->GetModule<ModuleFileSystem>()->ZipLibFolder();

	AddConfigToZip(startingScene);

	LOG_INFO("Done creating ZIP!");
}

} // namespace

void BuildGame(BuildType buildType, bool generateZip, const std::string& startingScene)
{
	std::wstring buildScriptPath = L"..\\Source\\BuildScripts\\";
	std::wstring buildScript = buildScriptPath;
	switch (buildType)
	{
		case BuildType::DEBUG_GAME:
			LOG_INFO("Building DebugGame...\n");
			buildScript += L"buildDebug";
			break;
		case BuildType::RELEASE_GAME:
			LOG_INFO("Building ReleaseGame...\n");
			buildScript += L"buildRelease";
			break;
	}
	buildScript += L".bat";

	compileThread = std::async(std::launch::async, &CompileGame, buildScript);
	if (generateZip)
	{
		zipThread = std::async(std::launch::async, &CreateZip, startingScene);
	}
	else
	{
		AddConfigToZip(startingScene);
	}
}

void Terminate()
{
	if (Compiling())
	{
		LOG_INFO("For now, you can't exit the engine while the game is compiling. Waiting on compilation to finish.");
		compileThread.get();
	}
	if (Zipping())
	{
		LOG_INFO(
			"For now, you can't exit the engine while the binaries are being zipped. Waiting on zipping to finish.");
		zipThread.get();
	}
}

bool Compiling()
{
	return compileThread.valid() && !compileThread._Is_ready();
}

bool Zipping()
{
	return zipThread.valid() && !zipThread._Is_ready();
}

} // namespace builder
