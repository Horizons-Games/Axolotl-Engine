#pragma once
#include "Module.h"
#include "Command.h"

#include <list>
#include <memory>
#include <utility>

class ModuleCommand : public Module
{
public:
	ModuleCommand();
	~ModuleCommand() override;

	bool Init();
	update_status Update();
	bool Cleanup();

	template<typename C, typename ...Args, std::enable_if_t<std::is_base_of<Command, C>::value, bool> = true >
	void CreateAndExecuteCommand(Args&& ...args);

	void Undo();
	void Redo();
private:
	std::list<std::unique_ptr<Command>> commandList;
	std::list<std::unique_ptr<Command>>::iterator commandListIterator;
	const static int commandLimit = 15;
};

template<typename C, typename ...Args, std::enable_if_t<std::is_base_of<Command, C>::value, bool>>
void ModuleCommand::CreateAndExecuteCommand(Args&& ...args)
{
	std::unique_ptr<Command> command = std::make_unique<C>(std::forward<Args>(args)...);
	commandList.emplace(commandListIterator, command->Execute());
	if (commandListIterator != std::end(commandList))
	{
		commandListIterator = commandList.erase(commandListIterator, std::end(commandList));
	}
	if (commandList.size() > commandLimit)
	{
		commandList.erase(std::begin(commandList));
	}
}