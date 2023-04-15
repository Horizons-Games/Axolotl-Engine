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
	delete comp;
	delete enable;
}

std::unique_ptr<Command> CommandComponentEnabled::Execute()
{
	if (desiredValue)
	{
		comp->Enable();
		*enable = true;
		std::unique_ptr<CommandComponentEnabled> reverse = std::make_unique<CommandComponentEnabled>(comp, enable, false);
		return reverse;
	}
	comp->Disable();
	*enable = false;
	std::unique_ptr<CommandComponentEnabled> reverse = std::make_unique<CommandComponentEnabled>(comp, enable, true);
	return reverse;
}