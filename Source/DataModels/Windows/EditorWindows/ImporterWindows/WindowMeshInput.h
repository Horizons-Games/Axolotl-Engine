#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "Globals.h"
#include <memory>

class ComponentMeshRenderer;

class WindowMeshInput :
	public WindowFileBrowser
{
public:
	WindowMeshInput(ComponentMeshRenderer* componentMesh);
	~WindowMeshInput() = default;
	void DoThisIfOk() override;

private:
	ComponentMeshRenderer* componentMesh;
};
