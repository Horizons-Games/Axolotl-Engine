#include "StdAfx.h"
#include "HackingCommand.h"


#include <map>
#include <SDL/include/SDL_gamecontroller.h>
#include <SDL/include/SDL_scancode.h>



HackingCommand::HackingCommand()
{
}

void HackingCommand::Start()
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

HackingCommandType HackingCommand::GetCommand(SDL_GameControllerButton key)
{
	auto it = buttonHackingMap.find(key);

	if (it != buttonHackingMap.end())
	{
		return it->second;
	}
	else
	{
		return HackingCommandType::COMMAND_X;
	}
}
