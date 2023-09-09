#pragma once

#include "Scripting\Script.h"
#include <vector>


class HackingCommand : public Script
{
public:
	HackingCommand();
	~HackingCommand() override = default;

	void Start() override;

	HackingCommands GetCommand(SDL_Scancode key);
	HackingCommands GetCommand(SDL_GameControllerButton key);
};

