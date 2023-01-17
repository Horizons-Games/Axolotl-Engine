#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowLoadScene : public WindowFileBrowser
{
public:
	WindowLoadScene();
	~WindowLoadScene() = default;
	void DoThisIfOk() override;
};

inline WindowLoadScene::WindowLoadScene() :WindowFileBrowser()
{
	dialogName = " Load Scene File";
	title = ICON_IGFD_FOLDER" Load Scene File";
	filters = SCENE_EXTENSION;
	startPath = "Assets/Scenes";
}