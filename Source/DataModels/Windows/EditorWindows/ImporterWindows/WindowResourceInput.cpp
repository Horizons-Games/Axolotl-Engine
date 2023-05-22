#include "WindowResourceInput.h"

#include "Application.h"
#include "DataModels/Resources/Resource.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleEditor.h"

WindowResourceInput::WindowResourceInput()
{
	dialogName = "Select Texture";
	title = "Load Resource";
	filters = "Resource Files (*.anim){.anim}";
	startPath = "Assets";
}

WindowResourceInput::~WindowResourceInput()
{
}

void WindowResourceInput::DoThisIfOk()
{
	this->isLoading = false;
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	std::shared_ptr<Resource> resource = App->GetModule<ModuleResources>()->RequestResource<Resource>(filePath);
	App->GetModule<ModuleEditor>()->SetResourceOnStateMachineEditor(resource);
}
