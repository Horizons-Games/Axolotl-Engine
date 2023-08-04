#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowVideoInput : public WindowFileBrowser
{
public:
	WindowVideoInput();
	~WindowVideoInput() override;

	void DoThisIfOk() override;
};