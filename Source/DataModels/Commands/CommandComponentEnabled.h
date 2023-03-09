#pragma once

#include "Command.h"
#include "DataModels/Components/Component.h"

class CommandComponentEnabled : public Command
{
public:
	CommandComponentEnabled(Component* comp, bool enabled);
	virtual ~CommandComponentEnabled() override;

	std::unique_ptr<Command> Execute() override;

private:
	Component* comp;
	bool enabled;
};

CommandComponentEnabled::CommandComponentEnabled(Component* comp, bool enabled) : comp(comp), enabled(enabled)
{}

CommandComponentEnabled::~CommandComponentEnabled()
{}

std::unique_ptr<Command> CommandComponentEnabled::Execute()
{
	if (enabled)
	{
		comp->Enable();
		std::unique_ptr<CommandComponentEnabled> reverse = std::make_unique<CommandComponentEnabled>(comp, false);
		return reverse;
	}
	comp->Disable();
	std::unique_ptr<CommandComponentEnabled> reverse = std::make_unique<CommandComponentEnabled>(comp, true);
	return reverse;
}