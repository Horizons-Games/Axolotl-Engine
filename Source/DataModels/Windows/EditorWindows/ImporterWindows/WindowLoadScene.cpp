#include "WindowLoadScene.h"

#include "Application.h"
#include "ModuleScene.h"

void WindowLoadScene::DoThisIfOk()
{
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	App->scene->LoadSceneFromJson(filePath);
}