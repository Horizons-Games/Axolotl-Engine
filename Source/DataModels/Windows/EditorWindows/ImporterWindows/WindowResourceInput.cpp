#include "WindowResourceInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleEditor.h"
#include "DataModels/Resources/Resource.h"

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
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	std::shared_ptr<Resource> resource = App->resources->RequestResource<Resource>(filePath);
	App->editor->SetResourceOnStateMachineEditor(resource);
}
