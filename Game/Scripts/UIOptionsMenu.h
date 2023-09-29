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
	struct HeaderOptionsButton
	{
		ComponentButton* button;
		GameObject* canvas;
		GameObject* hovered;
	};

	struct ButtonOptionInfo
	{
		int canvasPosition;
		int buttonPosition;
		int optionPosition;
	};

	std::vector<HeaderOptionsButton> buttonsAndCanvas;
	std::vector<ButtonOptionInfo> actualConfig;
	std::vector<ButtonOptionInfo> previousConfig;

	int headerMenuPosition;
	int newHeaderMenuPosition;
	int selectedOption;
	int actualButton;
	int previousButton;
	int actualButtonHover;
	int maxButtonsOptions;
	int maxOptions;
	int newSelectedOption;
	int saveSelectedOption;

	ModuleInput* input;
	ModuleUI* ui;
	ModuleWindow* window;
	ModuleRender* render;
	ModuleAudio* audio;
	JoystickVerticalDirection verticalDirection;

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









