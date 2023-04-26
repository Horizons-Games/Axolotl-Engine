#include "WindowMaterialInput.h"

#include "Application.h"
#include "Components/ComponentMeshRenderer.h"

#include "FileSystem/ModuleResources.h"
#include "DataModels/Components/ComponentMeshRenderer.h"
#include "Resources/ResourceMaterial.h"

WindowMaterialInput::WindowMaterialInput(ComponentMeshRenderer* componentMeshRenderer) :
	WindowFileBrowser(), componentMeshRenderer(componentMeshRenderer)
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
	if (componentMeshRenderer)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceMaterial> material = App->GetModule<ModuleResources>()->RequestResource<ResourceMaterial>(filePath);
		componentMeshRenderer->SetMaterial(material);
	}
}
