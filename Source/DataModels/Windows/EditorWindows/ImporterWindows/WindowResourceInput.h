#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowResourceInput :
    public WindowFileBrowser
{
public:
	WindowResourceInput();
	~WindowResourceInput() override;

	void DoThisIfOk() override;
};
