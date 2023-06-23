#pragma once

#include "Scripting\Script.h"

class ComponentButton;
class ComponentScript;
class UIGameManager;

class UIOptionsMenu : public Script
{
public:
	UIOptionsMenu();
	~UIOptionsMenu() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:

	GameObject* gameOption;
	GameObject* videoOption;
	GameObject* audioOption;
	GameObject* hudOption;
	GameObject* keysOption;
	ComponentButton* buttonComponent;
	UIGameManager* UIGameManagerClass;
};









