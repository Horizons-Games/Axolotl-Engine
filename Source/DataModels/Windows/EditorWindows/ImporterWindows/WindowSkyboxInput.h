#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowSkyboxInput : public WindowFileBrowser
{
public:
	WindowSkyboxInput();
	~WindowSkyboxInput() override;

	void DoThisIfOk() override;
};