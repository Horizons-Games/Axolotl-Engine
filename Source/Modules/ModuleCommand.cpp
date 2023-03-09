#include "Application.h"

#include "ModuleCommand.h"
#include "ModuleInput.h"

ModuleCommand::ModuleCommand() : commandListIterator(std::begin(commandList))
{
}

ModuleCommand::~ModuleCommand()
{
}

bool ModuleCommand::Init()
{
	return true;
}

update_status ModuleCommand::Update()
{
	if (
		App->input->GetKey(SDL_SCANCODE_LCTRL) != KeyState::IDLE
		&& App->input->GetKey(SDL_SCANCODE_Z) != KeyState::IDLE)
	{
		Undo();
	}
	else if (
		App->input->GetKey(SDL_SCANCODE_LCTRL) != KeyState::IDLE
		&& App->input->GetKey(SDL_SCANCODE_Y) != KeyState::IDLE)
	{
		Redo();
	}
	return UPDATE_CONTINUE;
}

bool ModuleCommand::Cleanup()
{
	return true;
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