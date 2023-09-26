#pragma once

#include "Scripting\Script.h"
#include <vector>

class ComponentButton;

class UIOptionsMenu : public Script
{
public:
	UIOptionsMenu();
	~UIOptionsMenu() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	struct OptionsButtonInfo
	{
		ComponentButton* button;
		GameObject* canvas;
		GameObject* hovered;
	};

	std::vector<OptionsButtonInfo> buttonsAndCanvas;
	int selectedPositon = 0;

	GameObject* gameOptionButton;
	GameObject* videoOptionButton;
	GameObject* audioOptionButton;
	GameObject* controlOptionButton;

	GameObject* gameOptionCanvas;
	GameObject* videoOptionCanvas;
	GameObject* audioOptionCanvas;
	GameObject* controlOptionCanvas;

	GameObject* gameOptionHover;
	GameObject* videoOptionHover;
	GameObject* audioOptionHover;
	GameObject* controlOptionHover;

	ComponentButton* gameOptionComponentButton;
	ComponentButton* videoOptionComponentButton;
	ComponentButton* audioOptionComponentButton;
	ComponentButton* controlOptionComponentButton;

};









