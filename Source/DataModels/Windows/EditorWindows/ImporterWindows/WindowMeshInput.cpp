#include "WindowMeshInput.h"

#include "Components/ComponentMeshRenderer.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMesh.h"

void WindowMeshInput::DoThisIfOk()
{
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	UID uidMesh = this->ImportResourceWithLoadingWindow(filePath);
	std::weak_ptr<ResourceMesh> mesh = App->resources->RequestResource<ResourceMesh>(uidMesh);
	componentMesh->SetMesh(mesh);
}
