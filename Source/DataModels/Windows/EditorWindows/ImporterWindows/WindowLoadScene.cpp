#include "Windows/EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "FontIcons/CustomFont.cpp"

WindowLoadScene::WindowLoadScene():WindowImporter()
{	
	dialogName = " Load Scene File";
	title = ICON_IGFD_FOLDER" Load Scene File";
	filters = ".scene";
	startPath = "./Assets/Scenes";
}
WindowLoadScene::~WindowLoadScene()
{}