#pragma once

#include "Scripting\Script.h"
#include <vector>

#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"

class ComponentButton;
class ComponentSlider;

enum class Canvas
{
	GAME_CANVAS = 0,
	VIDEO_CANVAS = 1,
	AUDIO_CANVAS = 2,
	CONTROLL_CANVAS = 3
};
enum class Button
{
	//GAME
	FPS = 0,
	VSYNC = 1,
	BRIGHTNESS = 2,
	RESOLUTION = 3,
	WINDOWSMODE = 4,

	//VIDEO

	//AUDIO
	GENERAL = 0,
	MUSIC = 1,
	VFX = 2,
};

class UIOptionsMenu : public Script
{
public:
	UIOptionsMenu();
	~UIOptionsMenu() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void ControlEnable();
	//void KeyboardEnable();

private:

	struct HeaderOptionsButton
	{
		ComponentButton* button;
		GameObject* canvas;
		GameObject* hovered;
	};

	struct ButtonOptionInfo
	{
		int actualOption;
		int defaultOption;
		bool locked;
	};
	struct CanvasOptionInfo
	{
		std::vector<ButtonOptionInfo> options;
	};

	std::vector<HeaderOptionsButton> buttonsAndCanvas;
	std::vector<CanvasOptionInfo> actualConfig;

	int headerMenuPosition;
	int newHeaderMenuPosition;
	int selectedOption;
	int actualButton;
	int previousButton;
	int actualButtonHover;
	int maxButtonsOptions;
	int maxOptions;
	int newSelectedOption;
	float valueSlider;

	bool isSlider;
	bool windowSize;
	bool optionSizeLock;
	bool resettButtonIndex;

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
	ComponentSlider* slider;

	void UpdateChanges();
	void ApplyChanges(int headerMenuPosition, int actualButton, int newSelectedOption);
	void InitOptionMenu();
	void LoadOptions();
	void SaveOptions();
	void LoadDefaultOptions();

	void GameOption(int button, int option);
	void VideoOption(int button, int option);
	void AudioOption(int button, int option);
	void ControlOption();

	void BackToLastSavedOption();
	bool IsSlider(int header, int button, int option);
	void IsSizeOptionEnable();
	void IsFpsEnable();

};









