#include "WindowMeshInput.h"

#include "Components/ComponentMeshRenderer.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMesh.h"

void WindowMeshInput::DoThisIfOk()
{
	std::shared_ptr<ComponentMeshRenderer> asShared = componentMesh.lock();
	if (asShared)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		UID uidMesh = App->resources->ImportResource(filePath);
		std::weak_ptr<ResourceMesh> mesh = App->resources->RequestResource<ResourceMesh>(uidMesh);
		asShared->SetMesh(mesh);
	}
}
