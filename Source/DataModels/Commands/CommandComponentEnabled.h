#pragma once

#include "Command.h"
#include "DataModels/Components/Component.h"

class CommandComponentEnabled : public Command
{
public:
	CommandComponentEnabled(Component* comp, bool desiredValue);
	virtual ~CommandComponentEnabled() override;

	std::unique_ptr<Command> Execute() override;

private:
	Component* comp;
	bool desiredValue;
};

CommandComponentEnabled::CommandComponentEnabled(Component* comp, bool desiredValue) : comp(comp), 
												desiredValue(desiredValue)
{
}

CommandComponentEnabled::~CommandComponentEnabled()
{
}

std::unique_ptr<Command> CommandComponentEnabled::Execute()
{
	desiredValue ? comp->Enable() : comp->Disable();
	std::unique_ptr<CommandComponentEnabled> reverse = 
		std::make_unique<CommandComponentEnabled>(comp, !desiredValue);
	return reverse;
}