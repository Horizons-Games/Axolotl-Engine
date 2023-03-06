#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowSaveScene : public WindowFileBrowser
{
public:
	WindowSaveScene();
	~WindowSaveScene() override;

	void DoThisIfOk() override;
};