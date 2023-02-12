#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "Globals.h"
#include <memory>

class ComponentMeshRenderer;

class WindowMeshInput :
	public WindowFileBrowser
{
public:
	WindowMeshInput(const std::weak_ptr<ComponentMeshRenderer>& componentMesh);
	~WindowMeshInput() = default;
	void DoThisIfOk() override;

private:
	std::weak_ptr<ComponentMeshRenderer> componentMesh;
};

inline WindowMeshInput::WindowMeshInput(const std::weak_ptr<ComponentMeshRenderer>& componentMesh) :
	WindowFileBrowser(),
	componentMesh(componentMesh)
{
	dialogName = "Select Mesh";
	title = "Load Mesh";
	filters = MESH_EXTENSION;
	startPath = "Assets/Meshes";
}

