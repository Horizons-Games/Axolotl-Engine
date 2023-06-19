#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentImage;

class WindowSpriteInput : public WindowFileBrowser
{
public:
	WindowSpriteInput(ComponentImage* image);
	~WindowSpriteInput() override;

	void DoThisIfOk() override;

private:
	ComponentImage* component;
};
