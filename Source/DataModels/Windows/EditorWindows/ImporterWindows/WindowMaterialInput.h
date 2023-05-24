#pragma once

#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentMeshRenderer;

class WindowMaterialInput : public WindowFileBrowser
{
public:
	WindowMaterialInput(ComponentMeshRenderer* window);
	~WindowMaterialInput() override;

	void DoThisIfOk() override;

private:
	ComponentMeshRenderer* componentMeshRenderer;

	friend class WindowComponentMeshRenderer;
};
