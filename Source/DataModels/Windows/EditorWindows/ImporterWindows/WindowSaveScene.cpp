#include "WindowSaveScene.h"

#include "Application.h"
#include "ModuleScene.h"

WindowSaveScene::WindowSaveScene() : WindowFileBrowser()
{
	isSave = true;
	dialogName = " Save Scene File";
	title = ICON_IGFD_SAVE" Save Scene File";
	filters = SCENE_EXTENSION;
	startPath = "Assets/Scenes";
}

WindowSaveScene::~WindowSaveScene()
{
}

void WindowSaveScene::DoThisIfOk()
{
	std::string filePathName = fileDialogImporter.GetCurrentFileName();
	App->scene->SaveSceneToJson(filePathName);
}