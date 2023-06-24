#pragma once

class Command
{
public:
	Command() = default;
	virtual ~Command() = default;

	virtual std::unique_ptr<Command> Execute() = 0;
};