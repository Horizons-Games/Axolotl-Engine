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
	return HackingCommandType::COMMAND_X;
}

HackingCommandType HackingCommand::GetCommand(SDL_GameControllerButton key)
{
	return HackingCommandType::COMMAND_X;
}
