#include "StdAfx.h"

#include "ComponentPlayerInput.h"

#include "Application.h"
#include "Modules/ModuleInput.h"

ComponentPlayerInput::ComponentPlayerInput(bool enabled, GameObject* owner) :
	Component(ComponentType::PLAYERINPUT, enabled, owner, true)
{
}

ComponentPlayerInput::~ComponentPlayerInput()
{
}

void ComponentPlayerInput::Update()
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	for (const auto& [gamepadButton, keyboardButton] : gamepadMapping)
	{
		input->SetKey(keyboardButton, input->GetGamepadButton(gamepadButton));
	}
}
