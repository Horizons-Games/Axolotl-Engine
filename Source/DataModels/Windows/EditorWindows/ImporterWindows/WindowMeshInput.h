#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "Globals.h"
#include <memory>

class ComponentMeshRenderer;

class WindowMeshInput :
	public WindowFileBrowser
{
public:
	//WindowMeshInput(const std::weak_ptr<ComponentMesh>& componentMesh);
	WindowMeshInput(ComponentMeshRenderer* componentMesh);
	~WindowMeshInput() = default;
	void DoThisIfOk() override;

private:
	//std::weak_ptr<ComponentMesh> componentMesh;
	ComponentMeshRenderer* componentMesh;
};

//inline WindowMeshInput::WindowMeshInput(const std::weak_ptr<ComponentMesh>& componentMesh) :WindowFileBrowser()
inline WindowMeshInput::WindowMeshInput(ComponentMeshRenderer* componentMesh) :WindowFileBrowser()
{
	this->componentMesh = componentMesh;

	dialogName = "Select Mesh";
	title = "Load Mesh";
	filters = MESH_EXTENSION;
	startPath = "Assets/Meshes";
}

