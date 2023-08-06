#pragma once

#include "Auxiliar/Generics/Updatable.h"
#include "DataModels/Components/Component.h"

class ComponentPlayerInput : public Component, public Updatable
{
public:
	ComponentPlayerInput(bool enabled, GameObject* owner);
	~ComponentPlayerInput() override;

	void Update() override;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	std::map<SDL_GameControllerButton, SDL_Scancode> gamepadMapping;
};
