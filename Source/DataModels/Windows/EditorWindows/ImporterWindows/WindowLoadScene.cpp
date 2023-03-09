#include "WindowLoadScene.h"

#include "Application.h"
#include "ModuleScene.h"

WindowLoadScene::WindowLoadScene() :WindowFileBrowser()
{
	dialogName = " Load Scene File";
	title = ICON_IGFD_FOLDER" Load Scene File";
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
	App->scene->LoadSceneFromJson(filePath);
}