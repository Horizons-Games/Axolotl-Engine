#include "StdAfx.h"
#include "UIOptionsMenu.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentSlider.h"
#include "Application.h"
#include "UIGameManager.h"



REGISTERCLASS(UIOptionsMenu);

UIOptionsMenu::UIOptionsMenu() : Script(), gameOptionButton(nullptr), videoOptionButton(nullptr), 
audioOptionButton(nullptr), controlsOptionButton(nullptr), gameOptionCanvas(nullptr), videoOptionCanvas(nullptr), 
audioOptionCanvas(nullptr), gameOptionHover(nullptr), videoOptionHover(nullptr), audioOptionHover(nullptr), 
controlsOptionHover(nullptr), gamepadTriggersImg(nullptr), headerMenuPosition(0), newHeaderMenuPosition(-1),
selectedOption(-1), actualButton(-1), actualButtonHover(-1), maxButtonsOptions(-1), maxOptions(-1), 
newSelectedOption(-1), loadFromMainMenu(false), valueSlider(-1), resetButtonIndex(true)
{
	REGISTER_FIELD(gameOptionButton, GameObject*);
	REGISTER_FIELD(videoOptionButton, GameObject*);
	REGISTER_FIELD(audioOptionButton, GameObject*);
	REGISTER_FIELD(controlsOptionButton, GameObject*);

	REGISTER_FIELD(gameOptionCanvas, GameObject*);
	REGISTER_FIELD(videoOptionCanvas, GameObject*);
	REGISTER_FIELD(audioOptionCanvas, GameObject*);
	REGISTER_FIELD(controlsOptionCanvas, GameObject*);

	REGISTER_FIELD(gameOptionHover, GameObject*);
	REGISTER_FIELD(videoOptionHover, GameObject*);
	REGISTER_FIELD(audioOptionHover, GameObject*);
	REGISTER_FIELD(controlsOptionHover, GameObject*);

	REGISTER_FIELD(gamepadTriggersImg, GameObject*);
}

void UIOptionsMenu::Init()
{
	input = App->GetModule<ModuleInput>();
	window = App->GetModule<ModuleWindow>();
	ui = App->GetModule<ModuleUI>();
	render = App->GetModule<ModuleRender>();
	audio = App->GetModule<ModuleAudio>();
	
	gameOptionComponentButton = gameOptionButton->GetComponent<ComponentButton>();
	videoOptionComponentButton = videoOptionButton->GetComponent<ComponentButton>();
	audioOptionComponentButton = audioOptionButton->GetComponent<ComponentButton>();
	controlsOptionComponentButton = controlsOptionButton->GetComponent<ComponentButton>();

	buttonsAndCanvas.push_back(HeaderOptionsButton{ gameOptionComponentButton, gameOptionCanvas, gameOptionHover });
	buttonsAndCanvas.push_back(HeaderOptionsButton{ videoOptionComponentButton, videoOptionCanvas, videoOptionHover });
	buttonsAndCanvas.push_back(HeaderOptionsButton{ audioOptionComponentButton, audioOptionCanvas, audioOptionHover });
	buttonsAndCanvas.push_back(HeaderOptionsButton{ controlsOptionComponentButton, controlsOptionCanvas, controlsOptionHover });

	LoadOptions();
}

void UIOptionsMenu::Start()
{
	gameOptionComponentButton->Disable();
	videoOptionComponentButton->Disable();
	audioOptionComponentButton->Disable();
	controlsOptionComponentButton->Disable();

	buttonsAndCanvas[0].canvas->Enable();
	buttonsAndCanvas[0].hovered->Enable();
	
	//InitOptionMenu();
	IsSizeOptionEnabled();
	IsFpsEnabled();
}

void UIOptionsMenu::Update(float deltaTime)
{
	ControllerMenuMode();
}

void UIOptionsMenu::ControllerMenuMode()
{
	if (resetButtonIndex)
	{
		ui->ResetCurrentButtonIndex();
		resetButtonIndex = false;
	}

	//BACK TO MAIN MENU
	if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
	{
		BackToLastSavedOption();
		SaveOptions();
		resetButtonIndex = true;
		return;
	}

	if (input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::DOWN)
	{
		BackToLastSavedOption();
		LoadDefaultOptions();
		resetButtonIndex = true;
		return;
	}

	// MOVE THE HEADER OF THE OPTIONS
	if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN)
	{
		newHeaderMenuPosition = headerMenuPosition;

		if (input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN)
		{
			if (headerMenuPosition > 0)
			{
				newHeaderMenuPosition--;
			}
		}
		else if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
		{
			if (headerMenuPosition < 3)
			{
				newHeaderMenuPosition++;
			}
		}

		//DISABLE THE HOVER BUTTON
		for (actualButtonHover = 0; actualButtonHover < 5; actualButtonHover++)
		{
			if (owner->GetChildren()[3]->GetChildren()[1]->GetChildren()[actualButtonHover]->IsEnabled())
			{
				owner->GetChildren()[3]->GetChildren()[1]->GetChildren()[actualButtonHover]->Disable();
				break;
			}
		}

		//ENABLE-DISABLE THE MAIN BAR MENU OPTION HOVER - GAME - VIDEO - AUDIO - CONTROLS
		buttonsAndCanvas[headerMenuPosition].canvas->Disable();
		buttonsAndCanvas[headerMenuPosition].hovered->Disable();
		BackToLastSavedOption();	
		headerMenuPosition = newHeaderMenuPosition;
		buttonsAndCanvas[newHeaderMenuPosition].canvas->Enable();
		buttonsAndCanvas[newHeaderMenuPosition].hovered->Enable();
		ui->ResetCurrentButtonIndex();
		newSelectedOption = -1;
	}

	//IF YOU DONT SAVE ANY OPTIONS THIS GO BACK TO THE LAST SAVED OPTION
	verticalDirection = input->GetLeftJoystickDirection().verticalDirection;
	if (verticalDirection == JoystickVerticalDirection::FORWARD || 
		verticalDirection == JoystickVerticalDirection::BACK)
	{
		BackToLastSavedOption();
	}

	maxButtonsOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren().size() - 1;

	//LOOK FOR THE current SELECTED BUTTON
	for (actualButton = 0; actualButton < maxButtonsOptions; actualButton++)
	{
		if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[0]->
			GetChildren()[0]->GetComponent<ComponentButton>()->IsHovered())
		{
			break;
		}
	}

	// IF THE BUTTON IS LOCKED BLOCK THE OPTIONS SELECCTION
	if (actualConfig[headerMenuPosition].options.size() == 0 || 
		actualConfig[headerMenuPosition].options[actualButton].locked)
	{
		return;
	}

	// MOVE LEFT OR RIGHT THE OPTIONS
	if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
	{
		maxOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->
			GetChildren()[1]->GetChildren().size() - 1;

		if (maxOptions >= 0)
		{
			for (selectedOption = 0; selectedOption < maxOptions; selectedOption++)
			{
				if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->
					GetChildren()[1]->GetChildren()[selectedOption]->IsEnabled())
				{
					break;
				}
			}

			isSlider = IsSlider(headerMenuPosition, actualButton, selectedOption);

			if (isSlider)
			{
				slider = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->
					GetChildren()[1]->GetChildren()[selectedOption]->GetComponent<ComponentSlider>();

				valueSlider = slider->GetCurrentValue();

				if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN)
				{
					if (valueSlider > slider->GetMinValue())
					{
						valueSlider -= 10.0f;
					}
				}
				else if (input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
				{
					if (valueSlider < slider->GetMaxValue())
					{
						valueSlider += 10.0f;
					}
				}
				if (valueSlider >= 0.0f && valueSlider <= 100.0f)
				{
					slider->ModifyCurrentValue(valueSlider);
					newSelectedOption = valueSlider;
					LOG_INFO("SLIDER {}", valueSlider);
				}
			}
			else // CHANGE OPTIONS WHEN IS NOT A SLIDER
			{
				newSelectedOption = selectedOption;

				if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN)
				{
					if (newSelectedOption > 0)
					{
						newSelectedOption--;
					}
				}
				else if (input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
				{
					if (newSelectedOption < maxOptions)
					{
						newSelectedOption++;
					}
				}

				//.ACTUAL CANVAS -> HOVERED BUTTON -> ALWAYS SECOND CHILDREN -> SELECTED OPTION
				// THINK THE IF IS USELESS BUT ITS WORKING GOOD SO DONT DELETE
				if (newSelectedOption != -1 && newSelectedOption != selectedOption)
				{
					buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->
						GetChildren()[1]->GetChildren()[selectedOption]->Disable();
					buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->
						GetChildren()[1]->GetChildren()[newSelectedOption]->Enable();
				}
			}
		}

	}

	if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN)
	{
		UpdateChanges();
	}

}

void UIOptionsMenu::UpdateChanges()
{
	if (actualConfig[headerMenuPosition].options[actualButton].actualOption == newSelectedOption || newSelectedOption == -1)
	{
		return;
	}

	actualConfig[headerMenuPosition].options[actualButton].actualOption = newSelectedOption;
	ApplyChanges(headerMenuPosition, actualButton, newSelectedOption);
}

void UIOptionsMenu::ApplyChanges(int headerMenuPosition, int actualButton, int newSelectedOption)
{
	switch (headerMenuPosition)
	{
	case 0:
		GameOption(actualButton, newSelectedOption);
		break;
	case 1:
		VideoOption(actualButton, newSelectedOption);
		break;
	case 2:
		AudioOption(actualButton, newSelectedOption);
		break;
	case 3:
		ControlsOption();
		break;
	default:
		break;
	}
}
void UIOptionsMenu::InitOptionMenu()
{
	CanvasOptionInfo gameCanvas;
	gameCanvas.options.push_back(ButtonOptionInfo{ 0, 0, false });
	gameCanvas.options.push_back(ButtonOptionInfo{ 0, 0, false });
	gameCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	gameCanvas.options.push_back(ButtonOptionInfo{ 0, 0, true });
	gameCanvas.options.push_back(ButtonOptionInfo{ 1, 1, false });
	actualConfig.push_back(gameCanvas);

	CanvasOptionInfo videoCanvas;
	videoCanvas.options.push_back(ButtonOptionInfo{ 1, 1, false });
	videoCanvas.options.push_back(ButtonOptionInfo{ 1, 1, false });
	videoCanvas.options.push_back(ButtonOptionInfo{ 1, 1, false });
	videoCanvas.options.push_back(ButtonOptionInfo{ 1, 1, false });
	actualConfig.push_back(videoCanvas);

	CanvasOptionInfo audioCanvas;
	audioCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	audioCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	audioCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	actualConfig.push_back(audioCanvas);

	CanvasOptionInfo controllerCanvas;
	actualConfig.push_back(controllerCanvas);
}
 

void UIOptionsMenu::LoadOptions()
{
	actualConfig.clear();
	std::string optionMenuPath = "Settings/OptionsConfig.txt";
	rapidjson::Document doc;
	Json optionsMenu(doc, doc);
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	char* buffer = {};
	fileSystem->Load(optionMenuPath.c_str(), buffer);
	optionsMenu.fromBuffer(buffer);
	delete buffer;

	for (int canvasIndex = 0; canvasIndex < optionsMenu.Size(); ++canvasIndex)
	{
		Json canvas = optionsMenu[canvasIndex];
		CanvasOptionInfo canvasInfo;
		for (int optionsIndex = 0; optionsIndex < canvas.Size(); ++optionsIndex)
		{
			ButtonOptionInfo buttonInfo;
			buttonInfo.actualOption = canvas[optionsIndex]["Actual_Option"];
			buttonInfo.defaultOption = canvas[optionsIndex]["Default_Option"];
			buttonInfo.locked = canvas[optionsIndex]["Locked_Option"];
			canvasInfo.options.push_back(buttonInfo);
			
				if (!IsSlider(canvasIndex, optionsIndex, 0))
				{
					buttonsAndCanvas[canvasIndex].canvas->GetChildren()[optionsIndex]->GetChildren()[1]->
						GetChildren()[buttonInfo.actualOption]->Enable();
				}
				else
				{
					ComponentSlider* sliderLoad;
					sliderLoad = buttonsAndCanvas[canvasIndex].canvas->GetChildren()[optionsIndex]->
						GetChildren()[1]->GetChildren()[0]->GetComponent<ComponentSlider>();
					sliderLoad->ModifyCurrentValue(buttonInfo.actualOption);
				}
			
		}
		actualConfig.push_back(canvasInfo);
	}

	// THE CANVAS CONTROLLER ONLY HAVE IMG INSIDE AND DONT SAVE BUTTONS - MADE AN ERROR ON THE SAVE AND LOAD.
	CanvasOptionInfo controller;
	actualConfig.push_back(controller);

	for (int canvasIndex = 0; canvasIndex < actualConfig.size(); ++canvasIndex)
	{
		for (int optionsIndex = 0; optionsIndex < actualConfig[canvasIndex].options.size(); ++optionsIndex)
		{
			ApplyChanges(canvasIndex, optionsIndex, actualConfig[canvasIndex].options[optionsIndex].actualOption);
		}
	}
}

void UIOptionsMenu::LoadDefaultOptions()
{
	for (int canvasIndex = 0; canvasIndex < actualConfig.size(); ++canvasIndex)
	{
		for (int optionsIndex = 0; optionsIndex < actualConfig[canvasIndex].options.size(); ++optionsIndex)
		{
			if (!IsSlider(canvasIndex, optionsIndex, 0))
			{
				buttonsAndCanvas[canvasIndex].canvas->GetChildren()[optionsIndex]->GetChildren()[1]->
					GetChildren()[actualConfig[canvasIndex].options[optionsIndex].actualOption]->Disable();
				buttonsAndCanvas[canvasIndex].canvas->GetChildren()[optionsIndex]->GetChildren()[1]->
					GetChildren()[actualConfig[canvasIndex].options[optionsIndex].defaultOption]->Enable();
			}
			else
			{
				ComponentSlider* sliderLoad;
				sliderLoad = buttonsAndCanvas[canvasIndex].canvas->GetChildren()[optionsIndex]->
					GetChildren()[1]->GetChildren()[0]->GetComponent<ComponentSlider>();
				sliderLoad->ModifyCurrentValue(actualConfig[canvasIndex].options[optionsIndex].defaultOption);
			}

			actualConfig[canvasIndex].options[optionsIndex].actualOption = actualConfig[canvasIndex].options[optionsIndex].defaultOption;
			ApplyChanges(canvasIndex, optionsIndex, actualConfig[canvasIndex].options[optionsIndex].defaultOption);
		}
	}
}

void UIOptionsMenu::SaveOptions()
{

	std::string optionMenuPath = "Settings/OptionsConfig.txt";
	rapidjson::Document doc;
	Json optionsMenu(doc, doc);
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	for (int canvasIndex = 0; canvasIndex < actualConfig.size()-1; ++canvasIndex)
	{
		Json canvas = optionsMenu[canvasIndex];
		for (int optionsIndex = 0; optionsIndex < actualConfig[canvasIndex].options.size(); ++optionsIndex)
		{
			canvas[optionsIndex]["Actual_Option"] = actualConfig[canvasIndex].options[optionsIndex].actualOption;
			canvas[optionsIndex]["Default_Option"] = actualConfig[canvasIndex].options[optionsIndex].defaultOption;
			canvas[optionsIndex]["Locked_Option"] = actualConfig[canvasIndex].options[optionsIndex].locked;
		}
	}

	rapidjson::StringBuffer buffer;
	optionsMenu.toBuffer(buffer);
	fileSystem->Save(optionMenuPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
}

void UIOptionsMenu::BackToLastSavedOption()
{
	if (newSelectedOption == -1)
	{
		return;
	}

	int saveSelectedOption = actualConfig[headerMenuPosition].options[actualButton].actualOption;
	
	if (isSlider)
	{
		slider->ModifyCurrentValue(saveSelectedOption);
		isSlider = false;
	}
	else
	{
		buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->
			GetChildren()[newSelectedOption]->Disable();
		buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->
			GetChildren()[saveSelectedOption]->Enable();
	}

	newSelectedOption = -1;
}

bool UIOptionsMenu::IsSlider(int header, int button, int option)
{
	return buttonsAndCanvas[header].canvas->GetChildren()[button]->GetChildren()[1]->GetChildren()[option]->
		HasComponent<ComponentSlider>();
}

void UIOptionsMenu::IsSizeOptionEnabled()
{
	float4 colorSet;

	int gameOptionWindowsMode = actualConfig[static_cast<int>(Canvas::GAME_CANVAS)].options[static_cast<int>(Button::WINDOWSMODE)].actualOption;

	if (gameOptionWindowsMode == 0 || gameOptionWindowsMode == 1)
	{
		colorSet = { 0.5f, 0.5f, 0.5f, 1.0f };
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->
			GetChildren()[static_cast<int>(Button::RESOLUTION)]->GetChildren()[1]->GetChildren()[1]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->
			GetChildren()[static_cast<int>(Button::RESOLUTION)]->GetChildren()[1]->GetChildren()[2]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->
			GetChildren()[static_cast<int>(Button::RESOLUTION)]->GetChildren()[1]->GetChildren()[3]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->
			GetChildren()[static_cast<int>(Button::RESOLUTION)]->GetChildren()[1]->GetChildren()[4]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->
			GetChildren()[static_cast<int>(Button::RESOLUTION)]->GetChildren()[1]->GetChildren()[0]->Enable();
		actualConfig[static_cast<int>(Canvas::GAME_CANVAS)].options[static_cast<int>(Button::RESOLUTION)].locked = true;
	}
	else
	{
		colorSet = { 1.0f, 1.0f, 1.0f, 1.0f };
		actualConfig[static_cast<int>(Canvas::GAME_CANVAS)].options[static_cast<int>(Button::RESOLUTION)].locked = false;

	}

	buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->GetChildren()[static_cast<int>(Button::RESOLUTION)]->
		GetChildren()[1]->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(colorSet);
}

void UIOptionsMenu::IsFpsEnabled()
{
	float4 colorSet;
	if (actualConfig[static_cast<int>(Canvas::GAME_CANVAS)].options[(int)Button::VSYNC].actualOption == 1)
	{
		colorSet = { 0.5f, 0.5f, 0.5f, 1.0f };
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->GetChildren()[static_cast<int>(Button::FPS)]->
			GetChildren()[1]->GetChildren()[1]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->GetChildren()[static_cast<int>(Button::FPS)]->
			GetChildren()[1]->GetChildren()[2]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->GetChildren()[static_cast<int>(Button::FPS)]->
			GetChildren()[1]->GetChildren()[3]->Disable();
		buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->GetChildren()[static_cast<int>(Button::FPS)]->
			GetChildren()[1]->GetChildren()[0]->Enable();
		actualConfig[static_cast<int>(Canvas::GAME_CANVAS)].options[static_cast<int>(Button::FPS)].locked = true;
	}
	else
	{
		colorSet = { 1.0f, 1.0f, 1.0f, 1.0f };
		actualConfig[static_cast<int>(Canvas::GAME_CANVAS)].options[static_cast<int>(Button::FPS)].locked = false;

	}

	buttonsAndCanvas[static_cast<int>(Canvas::GAME_CANVAS)].canvas->GetChildren()[static_cast<int>(Button::FPS)]->
		GetChildren()[1]->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(colorSet);
}

void UIOptionsMenu::GameOption(int button, int option)
{
	switch (button)
	{
	float brightnessToShow;
	case 0: //FPS LIMIT
		switch (option)
		{
		case 0:
			App->SetMaxFrameRate(300);
			LOG_INFO("SetMaxFrameRate: AUTO");
			break;
		case 1:
			App->SetMaxFrameRate(30);
			LOG_INFO("SetMaxFrameRate: 30");
			break;
		case 2:
			App->SetMaxFrameRate(60);
			LOG_INFO("SetMaxFrameRate: 60");
			break;
		case 3:
			App->SetMaxFrameRate(144);
			LOG_INFO("SetMaxFrameRate: 144");
			break;
		default:
			break;
		}
		break;
	case 1: //VSYNC
		switch (option)
		{
		case 0:
			window->SetVsync(false);
			LOG_INFO("SetVsync: false");
			break;
		case 1:
			window->SetVsync(true);
			LOG_INFO("SetVsync: true");
			break;
		default:
			break;
		}
			IsFpsEnabled();
		break;
	case 2: //BRIGHTNESS
		brightnessToShow = option;
		if (brightnessToShow * 0.01f < 0.3f)
		{
			window->SetBrightness(0.3f);
		}
		else
		{
			window->SetBrightness(brightnessToShow * 0.01f);
		}
		LOG_INFO("BRIGHTNESS {}", brightnessToShow * 0.01f);
		break;
	case 3: //RESOLUTION
		switch (option)
		{
		case 0:
			break;
		case 1:
			window->SetWindowSize(1280, 720);
			break;
		case 2:
			window->SetWindowSize(1920, 1080);
			break;
		case 3:
			window->SetWindowSize(2560, 1440);
			break;
		case 4:
			window->SetWindowSize(3840, 2160);
			break;
		default:
			break;
		}
		break;
	case 4: //WINDOWS MODE
		switch (option)
		{
		case 0:

			window->SetDesktopFullscreen(true);
			//window->SetFullscreen(true); // NOT WORKING PROPRERLY WE NEED TO FIX IT
			LOG_INFO("Windows Mode: Fullscreen");
			break;
		case 1:
			window->SetDesktopFullscreen(true);
			LOG_INFO("Windows Mode: DesktopFullscreen");
			break;
		case 2:
			window->SetBorderless(true);
			LOG_INFO("Windows Mode: Borderless");
			break;
		case 3:
			window->SetResizable(true);
			LOG_INFO("Windows Mode: Border");
			break;
		default:
			break;
		}
		IsSizeOptionEnabled();
		break;
	default:
		break;
	}

}
void UIOptionsMenu::VideoOption(int button, int option)
{
	switch (button)
	{
	case 0:
		render->ToggleShadows();
		break;
	case 1:
		render->ToggleSSAO();
		break;
	case 2:
		render->ToggleVSM();
		break;
	case 3:
		render->SwitchBloomActivation();
		break;
	default:
		break;
	}
}

void UIOptionsMenu::AudioOption(int button, int option)
{
	switch (button)
	{
	case 0: //MASTER
		// audio-> this is ModuleAudio
		break;
	case 1: //MUSIC
		break;
	case 2: // SFX
		break;
	default:
		break;
	}

}
void UIOptionsMenu::ControlsOption()
{
	//Function reserved to the control canvas options
}

void UIOptionsMenu::SetLoadFromMainMenu(bool fromMainMenu)
{
	loadFromMainMenu = fromMainMenu;
}

bool UIOptionsMenu::IsLoadFromMainMenu() const
{
	return loadFromMainMenu;
}


/*
void UIOptionsMenu::KeyboardMenuMode()
{
	gameOptionComponentButton->Enable();
	videoOptionComponentButton->Enable();
	audioOptionComponentButton->Enable();
	controlOptionComponentButton->Enable();
	padTriggersIMG->Disable();

	for (int i = 0; i < buttonsAndCanvas.size(); ++i)
	{
		ComponentButton* button = buttonsAndCanvas[i].button;
		if (button->IsHovered() && headerMenuPosition != i)
		{
			buttonsAndCanvas[i].hovered->Enable();
		}
		else
		{
			buttonsAndCanvas[i].hovered->Disable();
		}
		if (button->IsClicked())
		{
			newHeaderMenuPosition = i;
		}
	}
	if (newHeaderMenuPosition != -1 && newHeaderMenuPosition != headerMenuPosition)
	{
		buttonsAndCanvas[headerMenuPosition].canvas->Disable();
		buttonsAndCanvas[headerMenuPosition].button->Enable();
		headerMenuPosition = newHeaderMenuPosition;
		buttonsAndCanvas[headerMenuPosition].canvas->Enable();
		buttonsAndCanvas[headerMenuPosition].button->Disable();
	}
}
*/

