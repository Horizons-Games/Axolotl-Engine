#pragma once

#include "Command.h"
#include "DataModels/Components/Component.h"

class CommandComponentEnabled : public Command
{
public:
	CommandComponentEnabled(Component* comp, bool* enable, bool desiredValue);
	virtual ~CommandComponentEnabled() override;

	std::unique_ptr<Command> Execute() override;

private:
	Component* comp;
	bool* enable;
	bool desiredValue;
};

CommandComponentEnabled::CommandComponentEnabled(Component* comp, bool* enable, bool desiredValue) : comp(comp), 
						enable(enable), desiredValue(desiredValue)
{}

CommandComponentEnabled::~CommandComponentEnabled()
{
}

std::unique_ptr<Command> CommandComponentEnabled::Execute()
{
	std::unique_ptr<CommandComponentEnabled> reverse;
	if (desiredValue)
	{
		comp->Enable();
		*enable = true;
		reverse = std::make_unique<CommandComponentEnabled>(comp, enable, false);
	}
	else
	{
		comp->Disable();
		*enable = false;
		reverse = std::make_unique<CommandComponentEnabled>(comp, enable, true);
	}
	return reverse;
}