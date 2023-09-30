#include "StdAfx.h"
#include "UIOptionsMenu.h"

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
padTriggersIMG (nullptr), headerMenuPosition(0), newHeaderMenuPosition(-1), selectedOption(-1), actualButton(-1),
actualButtonHover(-1), maxButtonsOptions(-1), maxOptions(-1), newSelectedOption(-1), saveSelectedOption(-1),
previousButton(-1), valueSlider(-1)
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
	ui->ResetCurrentButtonIndex();
}

void UIOptionsMenu::Update(float deltaTime)
{
	ControlEnable();
}

void UIOptionsMenu::ControlEnable()
{
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
	}

	//IF YOU DONT SAVE ANY OPTIONS THIS GO BACK TO THE LAST SAVED OPTION
	verticalDirection = input->GetLeftJoystickDirection().verticalDirection;
	if (verticalDirection == JoystickVerticalDirection::FORWARD || verticalDirection == JoystickVerticalDirection::BACK)
	{
		BackToLastSavedOption();
	}

	// MOVE LEFT OR RIGHT THE OPTIONS
	if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN)
	{
		maxButtonsOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren().size() - 1;

		//LOOK FOR THE current SELECTED BUTTON
		if (maxButtonsOptions > 0)
		{
			for (actualButton = 0; actualButton < maxButtonsOptions; actualButton++)
			{
				if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[0]->GetChildren()[0]->GetComponent<ComponentButton>()->IsHovered())
				{
					break;
				}
			}
		}

		maxOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren().size();

		//LOOK FOR THE ACTUAL OPTION ENABLE
		if (maxOptions > 0)
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
				
				if (saveSelectedOption == -1)
				{
					saveSelectedOption = valueSlider;
				}

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
						valueSlider+= 10.0f;
					}
				}
				if (valueSlider >= 0.0f && valueSlider <= 100.0f)
				{
					slider->ModifyCurrentValue(valueSlider);
					saveSelectedOption = valueSlider;
				}
			}
			else // CAMBIA OPCIONES CUANDO NO ES UN SLIDER
			{
				if (saveSelectedOption == -1)
				{
					saveSelectedOption = selectedOption;
				}

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
					buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[selectedOption]->Disable();
					buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[newSelectedOption]->Enable();
				}
			}
		}
	}

	if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN)
	{
		if (isSlider)
		{
			saveSelectedOption = newSelectedOption;
		}

		SaveOption(headerMenuPosition, actualButton, saveSelectedOption);

		switch (headerMenuPosition)
		{
		case 0:
			GameOption(actualButton, saveSelectedOption);
		case 1:
			VideoOption();
		case 2:
			AudioOption();
		case 3:
			ControlOption();
		default:
			break;
		}
	}

}

int UIOptionsMenu::LookSavedOptions(int button)
{
	int savedOptionPosition = -1;

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
}

void UIOptionsMenu::SaveOption(int header, int button, int option)
{
	int savedOptionPosition = -1;

	savedOptionPosition = LookSavedOptions(button);

	if (savedOptionPosition == -1)
	{
		actualConfig.push_back(ButtonOptionInfo{ header, button, option });
	}
	else
	{
		actualConfig[savedOptionPosition].optionPosition = option;
	}
}

void UIOptionsMenu::GameOption(int button, int option)
{
	switch (button)
	{
	case 0: //FPS LIMIT

	case 1: //VSYNC
		switch (option)
		{
		case 0:
			window->SetVsync(false);
		case 1:
			window->SetVsync(true);
		default:
			break;
		}
	case 2: //BRIGHTNESS
		window->SetBrightness(0.50f);
	case 3: //RESOLUTION

	case 4: //WINDOWS MODE


	default:
		break;
	}
	//WINDOWS MODE
	//window->SetFullscreen(true);
	//window->SetDesktopFullscreen(true);
	//window->SetBorderless(true);
	//window->SetBrightness();

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

void UIOptionsMenu::BackToLastSavedOption()
{
	if (saveSelectedOption != -1)
	{
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
		saveSelectedOption = -1;
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


