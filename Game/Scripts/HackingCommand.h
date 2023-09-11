#pragma once

#include <map>
#include <SDL/include/SDL_gamecontroller.h>
#include <SDL/include/SDL_scancode.h>

enum HackingCommandType
{
	COMMAND_A,
	COMMAND_B,
	COMMAND_X,
	COMMAND_Y,
};



class HackingCommand
{
public:

	HackingCommand();

	static const std::pair<SDL_Scancode, SDL_GameControllerButton>* FromCommand(HackingCommandType command);

	static const std::vector<HackingCommandType> allHackingCommands;

private:

	static const std::map<HackingCommandType, std::pair<SDL_Scancode, SDL_GameControllerButton>> commandHackingMap;
};
