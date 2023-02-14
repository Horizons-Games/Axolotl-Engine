#include "WindowMaterialInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMaterial.h"
#include "Components/ComponentMaterial.h"

WindowMaterialInput::WindowMaterialInput(ComponentMaterial* componentMaterial) :
	WindowFileBrowser(),
	componentMaterial(componentMaterial)
{
	dialogName = "Select Material";
	title = "Load Material";
	filters = MATERIAL_EXTENSION;
	startPath = "Assets/Materials";
}

void WindowMaterialInput::DoThisIfOk()
{
	if (componentMaterial)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		UID uidMaterial = App->resources->ImportResource(filePath);
		std::weak_ptr<ResourceMaterial> material = App->resources->RequestResource<ResourceMaterial>(uidMaterial);
		componentMaterial->SetMaterial(material.lock() /*TODO: change with filesystem rework*/);
	}
}
