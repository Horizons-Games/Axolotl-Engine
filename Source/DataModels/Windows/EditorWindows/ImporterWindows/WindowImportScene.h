#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
class WindowImportScene : public WindowFileBrowser
{
public:
	WindowImportScene();
	~WindowImportScene() override;

	void DoThisIfOk() override;
};

