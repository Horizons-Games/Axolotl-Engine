#include "WindowSaveScene.h"

#include "Application.h"
#include "ModuleScene.h"

void WindowSaveScene::DoThisIfOk()
{
	std::string filePathName = fileDialogImporter.GetCurrentFileName();
	App->scene->SaveSceneToJson(filePathName);
}