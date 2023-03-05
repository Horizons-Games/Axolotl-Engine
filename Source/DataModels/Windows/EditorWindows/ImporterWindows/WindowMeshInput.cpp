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
	if (componentMesh && this->futureResourceUID.valid())
	{
		UID uidMesh = this->futureResourceUID.get();
		std::weak_ptr<ResourceMesh> mesh = App->resources->RequestResource<ResourceMesh>(uidMesh);
		componentMesh->SetMesh(mesh.lock() /*TODO: change with filesystem rework*/);
	}
}
