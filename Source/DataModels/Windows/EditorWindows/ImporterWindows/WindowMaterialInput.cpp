#include "WindowMaterialInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMaterial.h"
#include "Components/ComponentMaterial.h"

void WindowMaterialInput::DoThisIfOk()
{
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	UID uidMaterial = this->ImportResourceAsync(filePath);
	std::weak_ptr<ResourceMaterial> material = App->resources->RequestResource<ResourceMaterial>(uidMaterial);
	componentMaterial->SetMaterial(material);
}
