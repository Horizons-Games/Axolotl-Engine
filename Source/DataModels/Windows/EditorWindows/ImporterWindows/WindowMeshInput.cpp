#include "WindowMeshInput.h"

#include "Components/ComponentMeshRenderer.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMesh.h"

WindowMeshInput::WindowMeshInput(ComponentMeshRenderer* componentMesh) :
	WindowFileBrowser(), componentMesh(componentMesh)
{
	dialogName = "Select Mesh";
	title = "Load Mesh";
	filters = MESH_EXTENSION;
	startPath = "Assets/Meshes";
}

WindowMeshInput::~WindowMeshInput()
{
}

void WindowMeshInput::DoThisIfOk()
{
	if (componentMesh)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		this->ImportResourceAsync(filePath);
	}
}

void WindowMeshInput::GetResourceAfterImport()
{
	if (componentMesh && this->futureResource.valid())
	{
		std::shared_ptr<ResourceMesh> mesh = std::dynamic_pointer_cast<ResourceMesh>(this->futureResource.get());
		componentMesh->SetMesh(mesh);
	}
}
