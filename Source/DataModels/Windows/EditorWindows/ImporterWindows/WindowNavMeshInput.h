#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowNavMeshInput : public WindowFileBrowser
{
public:
	WindowNavMeshInput();
	~WindowNavMeshInput() override;

	void DoThisIfOk() override;
};
