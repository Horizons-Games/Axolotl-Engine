#include "WindowStateMachineInput.h"

#include "Components/ComponentAnimation.h"
#include "Resources/ResourceStateMachine.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowStateMachineInput::WindowStateMachineInput(ComponentAnimation* state) :
	WindowFileBrowser(), component(state)
{
	dialogName = "Select StateMachine";
	title = "Load StateMachine";
	filters = STATEMACHINE_EXTENSION;
	startPath = "Assets/StateMachine";
}

WindowStateMachineInput::~WindowStateMachineInput()
{
}


void WindowStateMachineInput::DoThisIfOk()
{
	if (component)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceStateMachine> texture = App->resources->RequestResource<ResourceStateMachine>(filePath);
		component->SetStateMachine(texture);
	}
}


