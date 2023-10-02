#include "StdAfx.h"
#include "UIOptionsMenu.h"
#include "Json.h"
#include "FileSystem/ModuleFileSystem.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentSlider.h"
#include "Application.h"
#include "UIGameManager.h"



REGISTERCLASS(UIOptionsMenu);

UIOptionsMenu::UIOptionsMenu() : Script(), gameOptionButton(nullptr), videoOptionButton(nullptr), audioOptionButton(nullptr),
controlOptionButton(nullptr), gameOptionCanvas(nullptr), videoOptionCanvas(nullptr), audioOptionCanvas(nullptr),
gameOptionHover(nullptr), videoOptionHover(nullptr), audioOptionHover(nullptr), controlOptionHover(nullptr),
padTriggersIMG(nullptr), headerMenuPosition(0), newHeaderMenuPosition(-1), selectedOption(-1), actualButton(-1),
actualButtonHover(-1), maxButtonsOptions(-1), maxOptions(-1), newSelectedOption(-1),
previousButton(-1), valueSlider(-1), optionSizeLock(false), resettButtonIndex(true)
{
	REGISTER_FIELD(gameOptionButton, GameObject*);
	REGISTER_FIELD(videoOptionButton, GameObject*);
	REGISTER_FIELD(audioOptionButton, GameObject*);
	REGISTER_FIELD(controlOptionButton, GameObject*);

	REGISTER_FIELD(gameOptionCanvas, GameObject*);
	REGISTER_FIELD(videoOptionCanvas, GameObject*);
	REGISTER_FIELD(audioOptionCanvas, GameObject*);
	REGISTER_FIELD(controlOptionCanvas, GameObject*);

	REGISTER_FIELD(gameOptionHover, GameObject*);
	REGISTER_FIELD(videoOptionHover, GameObject*);
	REGISTER_FIELD(audioOptionHover, GameObject*);
	REGISTER_FIELD(controlOptionHover, GameObject*);

	REGISTER_FIELD(padTriggersIMG, GameObject*);
}

void UIOptionsMenu::Start()
{
	input = App->GetModule<ModuleInput>();
	window = App->GetModule<ModuleWindow>();
	ui = App->GetModule<ModuleUI>();
	render = App->GetModule<ModuleRender>();
	audio = App->GetModule<ModuleAudio>();

	Assert(gameOptionButton != nullptr, axo::Format("Script owned by {} is fucked!!", owner));

	gameOptionComponentButton = gameOptionButton->GetComponent<ComponentButton>();
	videoOptionComponentButton = videoOptionButton->GetComponent<ComponentButton>();
	audioOptionComponentButton = audioOptionButton->GetComponent<ComponentButton>();
	controlOptionComponentButton = controlOptionButton->GetComponent<ComponentButton>();
	buttonsAndCanvas.push_back(HeaderOptionsButton{ gameOptionComponentButton, gameOptionCanvas, gameOptionHover });
	buttonsAndCanvas.push_back(HeaderOptionsButton{ videoOptionComponentButton, videoOptionCanvas, videoOptionHover });
	buttonsAndCanvas.push_back(HeaderOptionsButton{ audioOptionComponentButton, audioOptionCanvas, audioOptionHover });
	buttonsAndCanvas.push_back(HeaderOptionsButton{ controlOptionComponentButton, controlOptionCanvas, controlOptionHover });
	gameOptionComponentButton->Disable();
	videoOptionComponentButton->Disable();
	audioOptionComponentButton->Disable();
	controlOptionComponentButton->Disable();
	buttonsAndCanvas[0].canvas->Enable();
	buttonsAndCanvas[0].hovered->Enable();
	InitOptionMenu();
	IsSizeOptionEnable();
}

void UIOptionsMenu::Update(float deltaTime)
{
	ControlEnable();
}

void UIOptionsMenu::ControlEnable()
{
	if (resettButtonIndex == true)
	{
		ui->ResetCurrentButtonIndex();
		resettButtonIndex = false;
	}

	//BACK TO MAIN MENU
	if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
	{
		owner->GetParent()->GetChildren()[3]->Enable();
		owner->Disable();
		ui->ResetCurrentButtonIndex();
		resettButtonIndex = true;
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
	if (verticalDirection == JoystickVerticalDirection::FORWARD || verticalDirection == JoystickVerticalDirection::BACK)
	{
		BackToLastSavedOption();
		newSelectedOption = -1;
	}

	maxButtonsOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren().size() - 1;

	//LOOK FOR THE current SELECTED BUTTON
	for (actualButton = 0; actualButton < maxButtonsOptions; actualButton++)
	{
		if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[0]->GetChildren()[0]->GetComponent<ComponentButton>()->IsHovered())
		{
			break;
		}
	}


	if (actualConfig[headerMenuPosition].options.size() == 0 || actualConfig[headerMenuPosition].options[actualButton].locked)
	{
		return;
	}

	// MOVE LEFT OR RIGHT THE OPTIONS
	if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN)
	{
		maxOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren().size() - 1;

		if (maxOptions >= 0)
		{
			for (selectedOption = 0; selectedOption < maxOptions; selectedOption++)
			{
				if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[selectedOption]->IsEnabled())
				{
					break;
				}
			}

			isSlider = IsSlider(headerMenuPosition, actualButton, selectedOption);

			if (isSlider)
			{
				slider = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[selectedOption]->GetComponent<ComponentSlider>();

				valueSlider = slider->GetCurrentValue();

				if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN)
				{
					if (valueSlider > 0.0f)
					{
						valueSlider -= 10.0f;
					}
				}
				else if (input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN)
				{
					if (valueSlider < 100.0f)
					{
						valueSlider += 10.0f;
					}
				}
				if (valueSlider >= 0.0f && valueSlider <= 100.0f)
				{
					slider->ModifyCurrentValue(valueSlider);
					newSelectedOption = valueSlider;
				}
			}
			else // CAMBIA OPCIONES CUANDO NO ES UN SLIDER
			{
				newSelectedOption = selectedOption;

				if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN)
				{
					if (newSelectedOption > 0)
					{
						newSelectedOption--;
					}
				}
				else if (input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN)
				{
					if (newSelectedOption < maxOptions)
					{
						newSelectedOption++;
					}
				}

				//.ACTUAL CANVAS -> HOVERED BUTTON -> ALWAYS SECOND CHILDREN -> SELECTED OPTION
				if (newSelectedOption != -1 && newSelectedOption != selectedOption)
				{
					if (headerMenuPosition == 0 && optionSizeLock == true && actualButton == 3)
					{
						// SIZE BUTTON IS DISABLE WJEN FULLSCREEN
					}
					else
					{
						buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[selectedOption]->Disable();
						buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[newSelectedOption]->Enable();
					}

				}
			}
		}

	}

	if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN)
	{
		ApplyChanges();
	}

}

void UIOptionsMenu::ApplyChanges()
{
	if (actualConfig[headerMenuPosition].options[actualButton].actualOption == newSelectedOption)
	{
		return;
	}

	actualConfig[headerMenuPosition].options[actualButton].actualOption = newSelectedOption;

	switch (headerMenuPosition)
	{
	case 0:
		GameOption(actualButton, newSelectedOption);
		break;
	case 1:
		VideoOption();
		break;
	case 2:
		AudioOption();
		break;
	case 3:
		ControlOption();
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
	actualConfig.push_back(videoCanvas);

	CanvasOptionInfo audioCanvas;
	audioCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	audioCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	audioCanvas.options.push_back(ButtonOptionInfo{ 100, 100, false });
	actualConfig.push_back(audioCanvas);

	CanvasOptionInfo controllerCanvas;
	actualConfig.push_back(controllerCanvas);
	//actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::FPS].actualOption;
}
 

int UIOptionsMenu::LookSavedOptions(int button)
{
	/*int savedOptionPosition = -1;

	if (actualConfig.size() > 0)
	{
		for (int a=0; a < actualConfig.size(); a++)
		{
			if (actualConfig[a].buttonPosition == button)
			{
				savedOptionPosition = a;
			}
		}
	}

	return savedOptionPosition;
	*/
	return 0;
}

void UIOptionsMenu::SaveOption(int header, int button, int option, bool locked)
{
	/*int savedOptionPosition = -1;

	std::string optionMenuPath = "Settings/OptionsConfig.txt";
	rapidjson::Document doc;
	Json optionsMenu(doc, doc);
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	savedOptionPosition = LookSavedOptions(button);

	if (savedOptionPosition == -1)
	{
		actualConfig.push_back(ButtonOptionInfo{ header, button, option, locked });
	}
	else
	{
		actualConfig[savedOptionPosition].optionPosition = option;
		actualConfig[savedOptionPosition].buttonLocked = locked;
		optionsMenu["GAME CANVAS"] = header;
		rapidjson::StringBuffer buffer;
		optionsMenu.toBuffer(buffer);
		fileSystem->Save(optionMenuPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
	}*/
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
		buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[newSelectedOption]->Disable();
		buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[saveSelectedOption]->Enable();
	}
}

bool UIOptionsMenu::IsSlider(int header, int button, int option)
{
	if (buttonsAndCanvas[header].canvas->GetChildren()[button]->GetChildren()[1]->GetChildren()[option]->HasComponent<ComponentSlider>())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UIOptionsMenu::IsSizeOptionEnable()
{
	float4 colorSet;
	if (actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::WINDOWSMODE].actualOption == 0 || 
		actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::WINDOWSMODE].actualOption == 1)
	{
		colorSet = { 0.5f, 0.5f, 0.5f, 1.0f };
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::RESOLUTION]->GetChildren()[1]->GetChildren()[1]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::RESOLUTION]->GetChildren()[1]->GetChildren()[2]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::RESOLUTION]->GetChildren()[1]->GetChildren()[3]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::RESOLUTION]->GetChildren()[1]->GetChildren()[4]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::RESOLUTION]->GetChildren()[1]->GetChildren()[0]->Enable();
		actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::RESOLUTION].locked = true;
	}
	else
	{
		colorSet = { 1.0f, 1.0f, 1.0f, 1.0f };
		actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::RESOLUTION].locked = false;

	}

	buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::RESOLUTION]->GetChildren()[1]->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(colorSet);
}

void UIOptionsMenu::IsFpsEnable()
{
	float4 colorSet;
	if (actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::VSYNC].actualOption == 1)
	{
		colorSet = { 0.5f, 0.5f, 0.5f, 1.0f };
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::FPS]->GetChildren()[1]->GetChildren()[1]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::FPS]->GetChildren()[1]->GetChildren()[2]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::FPS]->GetChildren()[1]->GetChildren()[3]->Disable();
		buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::FPS]->GetChildren()[1]->GetChildren()[0]->Enable();
		actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::FPS].locked = true;
	}
	else
	{
		colorSet = { 1.0f, 1.0f, 1.0f, 1.0f };
		actualConfig[(int)Canvas::GAME_CANVAS].options[(int)Button::FPS].locked = false;

	}

	buttonsAndCanvas[(int)Canvas::GAME_CANVAS].canvas->GetChildren()[(int)Button::FPS]->GetChildren()[1]->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(colorSet);
}

void UIOptionsMenu::GameOption(int button, int option)
{
	float brightnessToShow = 0.0f;
	switch (button)
	{
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
		IsFpsEnable();
		break;
	case 2: //BRIGHTNESS
		brightnessToShow = option;
		window->SetBrightness(brightnessToShow * 0.01f);
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
			window->SetWindowSize(38240, 2160);
			break;
		default:
			break;
		}
		break;
	case 4: //WINDOWS MODE
		switch (option)
		{
		case 0:
			window->SetFullscreen(true);
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
			break;
		default:
			break;
		}
		IsSizeOptionEnable();
		break;
	default:
		break;
	}

}
void UIOptionsMenu::VideoOption()
{

}
void UIOptionsMenu::AudioOption()
{

}
void UIOptionsMenu::ControlOption()
{

}
/*
void UIOptionsMenu::KeyboardEnable()
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

