#include "StdAfx.h"

#include "ComponentPlayerInput.h"

#include "Application.h"
#include "Modules/ModuleInput.h"

namespace
{
const std::map<SDL_GameControllerButton, std::variant<SDL_Scancode, Uint8>> defaultGamepadMapping = {
	{ SDL_CONTROLLER_BUTTON_A, SDL_SCANCODE_SPACE },
	{ SDL_CONTROLLER_BUTTON_B, static_cast<Uint8>(SDL_BUTTON_LEFT) },
	{ SDL_CONTROLLER_BUTTON_X, SDL_SCANCODE_E },
	{ SDL_CONTROLLER_BUTTON_Y, static_cast<Uint8>(SDL_BUTTON_RIGHT) },
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
	for (const auto& [gamepadButton, keyboardButton] : defaultGamepadMapping)
	{
		KeyState gamepadButtonState = input->GetGamepadButton(gamepadButton);
		if (gamepadButtonState == KeyState::IDLE)
		{
			continue;
		}
		if (std::holds_alternative<SDL_Scancode>(keyboardButton))
		{
			input->SetKey(std::get<SDL_Scancode>(keyboardButton), gamepadButtonState);
		}
		else if (std::holds_alternative<Uint8>(keyboardButton))
		{
			Uint8 mouseButton = std::get<Uint8>(keyboardButton);
			input->SetMouseButton(mouseButton, gamepadButtonState);
		}
	}
}

void ComponentPlayerInput::InternalSave(Json& meta)
{
}

void ComponentPlayerInput::InternalLoad(const Json& meta)
{
}
