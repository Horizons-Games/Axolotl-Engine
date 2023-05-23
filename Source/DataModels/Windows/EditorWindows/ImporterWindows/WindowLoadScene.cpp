#include "WindowLoadScene.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Auxiliar/Utils/ConvertU8String.h"

WindowLoadScene::WindowLoadScene() :WindowFileBrowser()
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
	App->GetModule<ModuleScene>()->LoadScene(filePath);
}