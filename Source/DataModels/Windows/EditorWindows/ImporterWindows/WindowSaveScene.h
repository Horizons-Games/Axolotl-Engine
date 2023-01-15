#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
class WindowSaveScene : public WindowFileBrowser
{
public:
	WindowSaveScene();
	~WindowSaveScene() = default;
	void DoThisIfOk(std::string fileNamePath) override;
};

inline WindowSaveScene::WindowSaveScene() :WindowFileBrowser()
{
	isSave = true;
	dialogName = " Save Scene File";
	title = ICON_IGFD_SAVE" Save Scene File";
	filters = ".scene";
	startPath = "./Assets/Scenes";
}