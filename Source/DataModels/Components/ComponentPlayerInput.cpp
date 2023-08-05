#include "StdAfx.h"

#include "ComponentPlayerInput.h"

#include "Application.h"
#include "Modules/ModuleInput.h"

namespace
{
const std::map<SDL_GameControllerButton, SDL_Scancode> defaultGamepadMapping = {
	{ SDL_CONTROLLER_BUTTON_A, SDL_SCANCODE_SPACE },
	// O has to be used everywhere as left mouse button
	// and P as right mouse button
	// because this map cannot hold mouse buttons
	{ SDL_CONTROLLER_BUTTON_B, SDL_SCANCODE_O },
	{ SDL_CONTROLLER_BUTTON_X, SDL_SCANCODE_E },
	{ SDL_CONTROLLER_BUTTON_Y, SDL_SCANCODE_P },
	{ SDL_CONTROLLER_BUTTON_BACK, SDL_SCANCODE_M },
	{ SDL_CONTROLLER_BUTTON_GUIDE, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_START, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_LEFTSTICK, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_RIGHTSTICK, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER, SDL_SCANCODE_LSHIFT },
	{ SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_SCANCODE_W },
	{ SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_SCANCODE_S },
	{ SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_SCANCODE_A },
	{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_SCANCODE_D },
	{ SDL_CONTROLLER_BUTTON_MISC1, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_PADDLE1, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_PADDLE2, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_PADDLE3, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_PADDLE4, SDL_SCANCODE_ESCAPE },
	{ SDL_CONTROLLER_BUTTON_TOUCHPAD, SDL_SCANCODE_ESCAPE },
};
}

ComponentPlayerInput::ComponentPlayerInput(bool enabled, GameObject* owner) :
	Component(ComponentType::PLAYERINPUT, enabled, owner, true),
	gamepadMapping(defaultGamepadMapping)
{
}

ComponentPlayerInput::~ComponentPlayerInput()
{
}

void ComponentPlayerInput::Update()
{
	// if (App->GetPlayState() != PlayState::RUNNING)
	if (!App->IsOnPlayMode())
	{
		return;
	}
	ModuleInput* input = App->GetModule<ModuleInput>();
	for (const auto& [gamepadButton, keyboardButton] : gamepadMapping)
	{
		input->SetKey(keyboardButton, input->GetGamepadButton(gamepadButton));
	}
}

void ComponentPlayerInput::InternalSave(Json& meta)
{
}

void ComponentPlayerInput::InternalLoad(const Json& meta)
{
}
