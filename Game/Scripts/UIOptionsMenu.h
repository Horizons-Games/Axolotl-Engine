#pragma once

#include "Scripting\Script.h"

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
	CONTROLS_CANVAS = 3
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
	SFX = 2,
};

class UIOptionsMenu : public Script
{
public:
	UIOptionsMenu();
	~UIOptionsMenu() override = default;

	void Initialize();
	void Start() override;
	void Update(float deltaTime) override;

	void ControllerMenuMode(float deltaTime);
	//void KeyboardMenuMode();
	void LoadOptions();
	
	void SetApplyChangesOnLoad(bool apply);
	bool IsApplyChangesOnLoad() const;

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
	int actualButtonHover;
	int maxButtonsOptions;
	int maxOptions;
	int newSelectedOption;
	float valueSlider;
	float timeSinceLastChange;

	bool isSlider;
	bool applyChangesOnLoad;

	ModuleInput* input;
	ModuleUI* ui;
	ModuleWindow* window;
	ModuleRender* render;
	ModuleAudio* audio;
	JoystickVerticalDirection verticalDirection;
	JoystickHorizontalDirection horizontalDirection;

	GameObject* gameOptionButton;
	GameObject* videoOptionButton;
	GameObject* audioOptionButton;
	GameObject* controlsOptionButton;

	GameObject* gameOptionCanvas;
	GameObject* videoOptionCanvas;
	GameObject* audioOptionCanvas;
	GameObject* controlsOptionCanvas;

	GameObject* gameOptionHover;
	GameObject* videoOptionHover;
	GameObject* audioOptionHover;
	GameObject* controlsOptionHover;

	GameObject* gamepadTriggersImg;

	ComponentButton* gameOptionComponentButton;
	ComponentButton* videoOptionComponentButton;
	ComponentButton* audioOptionComponentButton;
	ComponentButton* controlsOptionComponentButton;
	ComponentSlider* slider;

	void UpdateChanges();
	void ApplyChanges(int headerMenuPosition, int actualButton, int newSelectedOption);
	void InitOptionMenu();
	void SaveOptions();
	void LoadDefaultOptions();

	void GameOption(int button, int option);
	void VideoOption(int button, int option);
	void AudioOption(int button, int option);
	void ControlsOption();

	void BackToLastSavedOption();
	bool IsSlider(int header, int button, int option);
	void IsSizeOptionEnabled();
	void IsFpsEnabled();

};







