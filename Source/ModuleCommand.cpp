#include "ModuleCommand.h"

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
	return UPDATE_CONTINUE;
}

bool ModuleCommand::Cleanup()
{
	return true;
}

void ModuleCommand::Undo()
{
	if (commandListIterator == commandList.begin()) return;
	--commandListIterator;
	*commandListIterator = commandListIterator->get()->Execute();
}

void ModuleCommand::Redo()
{
	if (commandListIterator == commandList.end()) return;
	*commandListIterator = commandListIterator->get()->Execute();
	++commandListIterator;
}