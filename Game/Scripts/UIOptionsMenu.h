#pragma once

#include "Scripting\Script.h"
#include <vector>

#include "ModuleInput.h"
#include "ModuleUI.h"

class ComponentButton;

class UIOptionsMenu : public Script
{
public:
	UIOptionsMenu();
	~UIOptionsMenu() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void ControlEnable();
	void KeyboardEnable();

	void GameOption();
	void VideoOption();
	void AudioOption();
	void ControlOption();

private:
	struct OptionsButtonInfo
	{
		ComponentButton* button;
		GameObject* canvas;
		GameObject* hovered;
	};

	std::vector<OptionsButtonInfo> buttonsAndCanvas;
	int headerMenuPosition = 0;
	int newHeaderMenuPosition = 0;
	int selectedOption = 0;
	int actualButtonHover = 0;

	ModuleInput* input;

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

	GameObject* padTriggersIMG;

	ComponentButton* gameOptionComponentButton;
	ComponentButton* videoOptionComponentButton;
	ComponentButton* audioOptionComponentButton;
	ComponentButton* controlOptionComponentButton;

};









