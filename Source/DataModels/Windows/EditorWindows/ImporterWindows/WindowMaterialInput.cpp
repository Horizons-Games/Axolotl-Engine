#include "WindowMaterialInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMaterial.h"
#include "Components/ComponentMaterial.h"

WindowMaterialInput::WindowMaterialInput(ComponentMaterial* componentMaterial) :
	WindowFileBrowser(), componentMaterial(componentMaterial)
{
	dialogName = "Select Material";
	title = "Load Material";
	filters = MATERIAL_EXTENSION;
	startPath = "Assets/Materials";
}

WindowMaterialInput::~WindowMaterialInput()
{
}

void WindowMaterialInput::DoThisIfOk()
{
	if (componentMaterial)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		this->ImportResourceAsync(filePath);
	}
}

void WindowMaterialInput::GetResourceAfterImport()
{
	if (componentMaterial && this->futureResource.valid())
	{
		std::shared_ptr<ResourceMaterial> material = std::dynamic_pointer_cast<ResourceMaterial>(this->futureResource.get());
		componentMaterial->SetMaterial(material);
	}
}
