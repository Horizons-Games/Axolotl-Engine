#include "WindowSaveScene.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Auxiliar/Utils/ConvertU8String.h"

WindowSaveScene::WindowSaveScene() : WindowFileBrowser()
{
	isSave = true;
	dialogName = " Save As...";
	title = ConvertU8String(ICON_IGFD_SAVE) + " Save As...";
	filters = SCENE_EXTENSION;
	startPath = "Assets/Scenes";
}

WindowSaveScene::~WindowSaveScene()
{
}

void WindowSaveScene::DoThisIfOk()
{
	this->isLoading = false;
	std::string filePathName = fileDialogImporter.GetCurrentFileName();
	App->scene->SaveSceneToJson(filePathName);
}