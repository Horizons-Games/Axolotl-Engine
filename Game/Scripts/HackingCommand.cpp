#include "StdAfx.h"
#include "HackingCommand.h"

//MAP FROM COMMAND TO KEY AND CONTROLLER BUTTON
const std::map<HackingCommandType, std::pair<SDL_Scancode, SDL_GameControllerButton>> HackingCommand::commandHackingMap =
{
	{ COMMAND_A , {SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A}},
	{ COMMAND_B , {SDL_SCANCODE_R, SDL_CONTROLLER_BUTTON_B}},
	{ COMMAND_X , {SDL_SCANCODE_E, SDL_CONTROLLER_BUTTON_X}},
	{ COMMAND_Y , {SDL_SCANCODE_T, SDL_CONTROLLER_BUTTON_Y}},
};

const std::vector<HackingCommandType> HackingCommand::allHackingCommands =
{ 
	COMMAND_A, 
	COMMAND_B, 
	COMMAND_X, 
	COMMAND_Y 
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
