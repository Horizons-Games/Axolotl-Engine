#pragma once

#include "Scripting\Script.h"
#include <vector>


enum HackingCommandType
{
	COMMAND_A,
	COMMAND_B,
	COMMAND_X,
	COMMAND_Y
};

const std::map<SDL_GameControllerButton, HackingCommandType> buttonHackingMap =
{
	{ SDL_CONTROLLER_BUTTON_A, COMMAND_A },
	{ SDL_CONTROLLER_BUTTON_B, COMMAND_B },
	{ SDL_CONTROLLER_BUTTON_X, COMMAND_X },
	{ SDL_CONTROLLER_BUTTON_Y, COMMAND_Y },
};

const std::map<SDL_Scancode, HackingCommandType> keyHackingMap =
{
	{ SDL_SCANCODE_SPACE, COMMAND_A },
	{ SDL_SCANCODE_R, COMMAND_B},
	{ SDL_SCANCODE_E, COMMAND_X},
	{ SDL_SCANCODE_T, COMMAND_Y },
};


class HackingCommand : public Script
{
public:
	HackingCommand();
	~HackingCommand() override = default;

	void Start() override;

	static HackingCommandType GetCommand(SDL_Scancode key);
	static HackingCommandType GetCommand(SDL_GameControllerButton key);
};

