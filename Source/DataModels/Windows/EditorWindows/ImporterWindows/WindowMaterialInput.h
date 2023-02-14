#pragma once

#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentMaterial;

class WindowMaterialInput : public WindowFileBrowser
{
public:
	WindowMaterialInput(ComponentMaterial* componentMaterial);
	~WindowMaterialInput() = default;
	void DoThisIfOk() override;

private:
	ComponentMaterial* componentMaterial;

	friend class WindowComponentMaterial;
};
