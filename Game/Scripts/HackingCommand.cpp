#include "StdAfx.h"
#include "HackingCommand.h"

const std::map<HackingCommandType, std::pair<SDL_Scancode, SDL_GameControllerButton>> HackingCommand::commandHackingMap =
{
	{ COMMAND_A , {SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A}},
	{ COMMAND_B , {SDL_SCANCODE_R, SDL_CONTROLLER_BUTTON_B}},
	{ COMMAND_X , {SDL_SCANCODE_E, SDL_CONTROLLER_BUTTON_X}},
	{ COMMAND_Y , {SDL_SCANCODE_T, SDL_CONTROLLER_BUTTON_Y}},
};

HackingCommand::HackingCommand()
{
}

const std::pair<SDL_Scancode, SDL_GameControllerButton>* HackingCommand::FromCommand(HackingCommandType command)
{
	auto it = commandHackingMap.find(command);

	if (it != commandHackingMap.end())
	{
		return &it->second;
	}
	else
	{
		return nullptr;
	}
}
