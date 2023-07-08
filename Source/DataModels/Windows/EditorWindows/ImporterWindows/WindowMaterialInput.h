#pragma once

#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowComponentMeshRenderer;

class WindowMaterialInput : public WindowFileBrowser
{
public:
	WindowMaterialInput(WindowComponentMeshRenderer* window);
	~WindowMaterialInput() override;

	void DoThisIfOk() override;

private:
	WindowComponentMeshRenderer* windowComponentMeshRenderer;

	friend class WindowComponentMeshRenderer;
};