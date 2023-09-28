#pragma once

#include "Scripting\Script.h"
#include <vector>

#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
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
	int newHeaderMenuPosition = -1;
	int selectedOption = -1;
	int actualButton = -1;
	int actualButtonHover = -1;
	int maxButtonsOptions = -1;
	int maxOptions = -1;
	int newSelectedOption = -1;

	ModuleInput* input;
	ModuleUI* ui;
	ModuleWindow* window;
	ModuleRender* render;
	ModuleAudio* audio;

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









