#include "WindowSaveScene.h"
#include "FontIcons/CustomFont.cpp"


WindowSaveScene::WindowSaveScene() :WindowFileBrowser()
{
	isSave = true;
	dialogName = " Save Scene File";
	title = ICON_IGFD_SAVE" Save Scene File";
	filters = ".scene";
	startPath = "./Assets/Scenes";
}
WindowSaveScene::~WindowSaveScene()
{}