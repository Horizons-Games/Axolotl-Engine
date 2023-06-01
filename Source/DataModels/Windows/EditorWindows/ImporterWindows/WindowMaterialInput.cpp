#include "WindowMaterialInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Windows/SubWindows/ComponentWindows/WindowComponentMeshRenderer.h"
#include "Resources/ResourceMaterial.h"

WindowMaterialInput::WindowMaterialInput(WindowComponentMeshRenderer* window) :
	WindowFileBrowser(), windowComponentMeshRenderer(window)
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
	if (windowComponentMeshRenderer)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceMaterial> material = App->GetModule<ModuleResources>()->RequestResource<ResourceMaterial>(filePath);
		windowComponentMeshRenderer->SetMaterial(material);
	}
}