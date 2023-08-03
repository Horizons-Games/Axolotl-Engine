#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowCubemapInput : public WindowFileBrowser
{
public:
	WindowCubemapInput();
	~WindowCubemapInput() override;

	void DoThisIfOk() override;
};