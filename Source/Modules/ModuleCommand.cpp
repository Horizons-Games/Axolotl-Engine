#include "Application.h"

#include "ModuleCommand.h"
#include "ModuleInput.h"

ModuleCommand::ModuleCommand() : commandListIterator(std::begin(commandList))
{
}

ModuleCommand::~ModuleCommand()
{
}

update_status ModuleCommand::Update()
{
	const ModuleInput* input = App->GetModule<ModuleInput>();
	if (input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT && input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
	{
		Undo();
	}
	else if (input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT && input->GetKey(SDL_SCANCODE_Y) == KeyState::DOWN)
	{
		Redo();
	}
	return update_status::UPDATE_CONTINUE;
}

void ModuleCommand::Undo()
{
	if (commandListIterator == commandList.begin())
	{
		return;
	}
	--commandListIterator;
	*commandListIterator = commandListIterator->get()->Execute();
}

void ModuleCommand::Redo()
{
	if (commandListIterator == commandList.end())
	{
		return;
	}
	*commandListIterator = commandListIterator->get()->Execute();
	++commandListIterator;
}