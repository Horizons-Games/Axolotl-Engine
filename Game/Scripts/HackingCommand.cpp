#include "StdAfx.h"
#include "HackingCommand.h"


#include <map>
#include <SDL/include/SDL_gamecontroller.h>
#include <SDL/include/SDL_scancode.h>



HackingCommand::HackingCommand()
{
}


HackingCommandType HackingCommand::GetCommand(SDL_Scancode key)
{
	auto it = keyHackingMap.find(key);

	if (it != keyHackingMap.end()) 
	{
		return it->second;
	}
	else 
	{
		return HackingCommandType::COMMAND_X;
	}
}


HackingCommandType HackingCommand::GetCommand(SDL_GameControllerButton button)
{
	auto it = buttonHackingMap.find(button);

	if (it != buttonHackingMap.end())
	{
		return it->second;
	}
	else
	{
		return HackingCommandType::COMMAND_X;
	}
}

const std::pair<SDL_Scancode, SDL_GameControllerButton>& HackingCommand::FromCommand(HackingCommandType command)
{
	auto it = commandHackingMap.find(command);

	if (it != commandHackingMap.end())
	{
		return it->second;
	}
	else
	{
		return { SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A };
	}
}