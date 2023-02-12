#pragma once

#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentMaterial;

class WindowMaterialInput : public WindowFileBrowser
{
public:
	WindowMaterialInput(const std::weak_ptr<ComponentMaterial>& componentMaterial);
	~WindowMaterialInput() = default;
	void DoThisIfOk() override;

private:
	std::weak_ptr<ComponentMaterial> componentMaterial;

	friend class WindowComponentMaterial;
};

inline WindowMaterialInput::WindowMaterialInput(const std::weak_ptr<ComponentMaterial>& componentMaterial) :
	WindowFileBrowser(),
	componentMaterial(componentMaterial)
{
	dialogName = "Select Material";
	title = "Load Material";
	filters = MATERIAL_EXTENSION;
	startPath = "Assets/Materials";
}
