#include "WindowMaterialInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMaterial.h"
#include "Components/ComponentMaterial.h"

void WindowMaterialInput::DoThisIfOk()
{
	std::shared_ptr<ComponentMaterial> asShared = componentMaterial.lock();
	if (asShared)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		UID uidMaterial = App->resources->ImportResource(filePath);
		std::weak_ptr<ResourceMaterial> material = App->resources->RequestResource<ResourceMaterial>(uidMaterial);
		asShared->SetMaterial(material);
	}
}
