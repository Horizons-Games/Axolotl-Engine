#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowLoadScene : public WindowFileBrowser
{
public:
	WindowLoadScene();
	~WindowLoadScene();

	void DoThisIfOk() override;
};