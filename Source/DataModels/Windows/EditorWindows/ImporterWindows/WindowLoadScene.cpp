#include "StdAfx.h"

#include "WindowLoadScene.h"

#include "Application.h"
#include "Auxiliar/Utils/ConvertU8String.h"
#include "ModuleScene.h"

#include "Defines/ExtensionDefines.h"

WindowLoadScene::WindowLoadScene() : WindowFileBrowser()
{
	dialogName = " Load Scene";
	title = ConvertU8String(ICON_IGFD_FOLDER) + " Load Scene";
	filters = SCENE_EXTENSION;
	startPath = "Assets/Scenes";
}

WindowLoadScene::~WindowLoadScene()
{
}

void WindowLoadScene::DoThisIfOk()
{
	this->isLoading = false;
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	bool doneLoading = false;
	App->GetModule<ModuleScene>()->LoadScene(filePath);
	return;
	App->GetModule<ModuleScene>()->LoadSceneAsync(filePath,
												  [&doneLoading]()
												  {
													  doneLoading = true;
												  });
	std::chrono::time_point currentTime = std::chrono::system_clock::now();
	while (!doneLoading)
	{
		float deltaTime =
			std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - currentTime).count();
		LOG_DEBUG("Loading... Time passed: {}", deltaTime);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}