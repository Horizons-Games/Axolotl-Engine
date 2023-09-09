#include "HackingCommand.h"
#include <map>
#include <SDL/include/SDL_gamecontroller.h>
#include <SDL/include/SDL_scancode.h>


enum HackingCommands
{
	COMMAND_A,
	COMMAND_B,
	COMMAND_X,
	COMMAND_Y
};
const std::map<SDL_GameControllerButton, HackingCommands> buttonHackingMap =
{
	{ SDL_CONTROLLER_BUTTON_A, COMMAND_A },
	{ SDL_CONTROLLER_BUTTON_B, COMMAND_B },
	{ SDL_CONTROLLER_BUTTON_X, COMMAND_X },
	{ SDL_CONTROLLER_BUTTON_Y, COMMAND_Y },
};

const std::map<SDL_Scancode, HackingCommands> keyHackingMap =
{
	{ SDL_SCANCODE_SPACE, COMMAND_A },
	{ SDL_SCANCODE_R, COMMAND_B},
	{ SDL_SCANCODE_E, COMMAND_X},
	{ SDL_SCANCODE_T, COMMAND_Y },
};

HackingCommand::HackingCommand()
{
}

void HackingCommand::Start()
{
}

HackingCommands HackingCommand::GetCommand(SDL_Scancode key)
{
	return HackingCommands();
}

HackingCommands HackingCommand::GetCommand(SDL_GameControllerButton key)
{
	return HackingCommands();
}
