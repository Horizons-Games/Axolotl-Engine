#include "WindowLoadScene.h"

#include "Application.h"
#include "ModuleScene.h"

WindowLoadScene::WindowLoadScene() : WindowFileBrowser()
{
	dialogName = " Load Scene";
	title = ICON_IGFD_FOLDER " Load Scene";
	filters = SCENE_EXTENSION;
	startPath = "Assets/Scenes";
}

WindowLoadScene::~WindowLoadScene()
{
}

void WindowLoadScene::DoThisIfOk()
{
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	App->scene->LoadSceneFromJson(filePath);
}