#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowLoadScene : public WindowFileBrowser
{
public:
	WindowLoadScene();
	~WindowLoadScene() = default;
	void DoThisIfOk(std::string fileNamePath) override;
};

inline WindowLoadScene::WindowLoadScene() :WindowFileBrowser()
{
	dialogName = " Load Scene File";
	title = ICON_IGFD_FOLDER" Load Scene File";
	filters = ".scene";
	startPath = "./Assets/Scenes";
}