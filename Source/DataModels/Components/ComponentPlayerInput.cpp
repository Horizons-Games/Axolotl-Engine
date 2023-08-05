#include "StdAfx.h"

#include "ComponentPlayerInput.h"

#include "Application.h"
#include "Modules/ModuleInput.h"

namespace
{
const std::map<SDL_GameControllerButton, SDL_Scancode> defaultGamepadMapping = {
	{ SDL_CONTROLLER_BUTTON_A, SDL_SCANCODE_SPACE },
	{ SDL_CONTROLLER_BUTTON_B, SDL_SCANCODE_Q },
	{ SDL_CONTROLLER_BUTTON_X, SDL_SCANCODE_V },
	{ SDL_CONTROLLER_BUTTON_Y, SDL_SCANCODE_LSHIFT },
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
