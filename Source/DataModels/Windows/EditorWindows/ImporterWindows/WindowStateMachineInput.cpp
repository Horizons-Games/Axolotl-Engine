#include "StdAfx.h"

#include "WindowStateMachineInput.h"

#include "Application.h"
#include "Components/ComponentAnimation.h"
#include "FileSystem/ModuleResources.h"
#include "Animation/StateMachine.h"

WindowStateMachineInput::WindowStateMachineInput(StateMachine* stateMachineInstance) :
	WindowFileBrowser(),
	stateMachineInstance(stateMachineInstance)
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
	if (stateMachineInstance)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceStateMachine> stateMachine =
			App->GetModule<ModuleResources>()->RequestResource<ResourceStateMachine>(filePath);
		stateMachineInstance->SetStateMachine(stateMachine);
	}
}
