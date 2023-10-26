#pragma once

#include "Scripting\Script.h"

#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"

class ComponentButton;
class ComponentSlider;

// using enum instead of enum class on purpose to avoid static_cast to int
enum Canvas
{
	GAME_CANVAS = 0,
	VIDEO_CANVAS = 1,
	AUDIO_CANVAS = 2,
	CONTROLS_CANVAS = 3
};

// using enum instead of enum class on purpose to avoid static_cast to int
enum Button
{
	// GAME
	FPS = 0,
	VSYNC = 1,
	BRIGHTNESS = 2,
	RESOLUTION = 3,
	WINDOWSMODE = 4,

	// VIDEO
	SHADOWS = 0,
	SSAO = 1,
	VSM = 2,
	BLOOM = 3,

	// AUDIO
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
	// void KeyboardMenuMode(float deltaTime);
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
	float timerFeedbackOption;

	bool isSlider;
	bool optionSizeLock;
	bool resetButtonIndex;
	bool applyChangesOnLoad;
	bool isSavingActive;

	ModuleInput* input;
	ModuleUI* ui;
	ModuleWindow* window;
	ModuleRender* render;
	ModuleAudio* audio;
	JoystickVerticalDirection verticalDirection;

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
	GameObject* backToMenuButton;
	GameObject* saveOptionsImg;

	ComponentButton* gameOptionComponentButton;
	ComponentButton* videoOptionComponentButton;
	ComponentButton* audioOptionComponentButton;
	ComponentButton* controlsOptionComponentButton;
	ComponentButton* buttonBackMenu;
	ComponentSlider* slider;
	ComponentSlider* sliderSaveOptions;

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
	void SaveOptionsFeedback(float deltaTime);
};
