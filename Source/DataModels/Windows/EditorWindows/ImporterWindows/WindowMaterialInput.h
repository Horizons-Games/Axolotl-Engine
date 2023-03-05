#pragma once

#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentMaterial;

class WindowMaterialInput : public WindowFileBrowser
{
public:
	WindowMaterialInput(ComponentMaterial* componentMaterial);
	~WindowMaterialInput() override;

	void DoThisIfOk() override;

protected:
	void GetResourceAfterImport() override;

private:
	ComponentMaterial* componentMaterial;

	friend class WindowComponentMaterial;
};
