#include "StdAfx.h"

#include "WindowImportScene.h"

#include "Application.h"
#include "Auxiliar/Utils/ConvertU8String.h"
#include "ModuleScene.h"

#include "Defines/ExtensionDefines.h"

WindowImportScene::WindowImportScene() : WindowFileBrowser()
{
	dialogName = " Import Scene";
	title = ConvertU8String(ICON_IGFD_FOLDER) + " Import Scene";
	filters = SCENE_EXTENSION;
	startPath = "Assets/Scenes";
}

WindowImportScene::~WindowImportScene()
{
}

void WindowImportScene::DoThisIfOk()
{
	this->isLoading = false;
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	App->GetModule<ModuleScene>()->LoadScene(filePath, true);
}