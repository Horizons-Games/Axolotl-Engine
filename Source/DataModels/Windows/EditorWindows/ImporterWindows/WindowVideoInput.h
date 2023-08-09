#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentVideo;

class WindowVideoInput : public WindowFileBrowser
{
public:
	WindowVideoInput(ComponentVideo* video);
	~WindowVideoInput() override;

	void DoThisIfOk() override;

private:
	ComponentVideo* component;
};