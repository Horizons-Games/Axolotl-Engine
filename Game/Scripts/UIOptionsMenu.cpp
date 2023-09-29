#include "StdAfx.h"
#include "UIOptionsMenu.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "Application.h"
#include "UIGameManager.h"



REGISTERCLASS(UIOptionsMenu);

UIOptionsMenu::UIOptionsMenu() : Script(), gameOptionButton(nullptr), videoOptionButton(nullptr), audioOptionButton(nullptr),
controlOptionButton(nullptr), gameOptionCanvas(nullptr), videoOptionCanvas(nullptr), audioOptionCanvas(nullptr), 
gameOptionHover(nullptr), videoOptionHover(nullptr), audioOptionHover(nullptr), controlOptionHover(nullptr), 
padTriggersIMG (nullptr), headerMenuPosition(0), newHeaderMenuPosition(-1), selectedOption(-1), actualButton(-1),
actualButtonHover(-1), maxButtonsOptions(-1), maxOptions(-1), newSelectedOption(-1), saveSelectedOption(-1), previousButton(-1)
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
	verticalDirection = input->GetLeftJoystickDirection().verticalDirection;

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
		headerMenuPosition = newHeaderMenuPosition;
		buttonsAndCanvas[newHeaderMenuPosition].canvas->Enable();
		buttonsAndCanvas[newHeaderMenuPosition].hovered->Enable();
		ui->ResetCurrentButtonIndex();
	}

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
	//IF YOU DONT SAVE ANY OPTIONS THIS GO BACK TO THE LAST SAVED OPTION
	if (saveSelectedOption != -1 && actualButton != previousButton && previousButton != -1)
	{
		//if (verticalDirection == JoystickVerticalDirection::FORWARD || verticalDirection == JoystickVerticalDirection::BACK)
		//{
		buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[previousButton]->GetChildren()[1]->GetChildren()[newSelectedOption]->Disable();
		buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[previousButton]->GetChildren()[1]->GetChildren()[saveSelectedOption]->Enable();
		saveSelectedOption = -1;
		previousButton = -1;
		newSelectedOption = -1;
		//}
	}
	// MOVE LEFT OR RIGHT THE OPTIONS
	if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN)
	{
		maxOptions = buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren().size() - 1;

		if (previousButton == -1)
		{
			previousButton = actualButton;
		}

		//LOOK FOR THE ACTUAL OPTION ENABLE
		if (maxOptions > 0)
		{
			for (selectedOption = 0; selectedOption < maxOptions; selectedOption++)
			{
				if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButton]->GetChildren()[1]->GetChildren()[selectedOption]->IsEnabled())
				{
					if (saveSelectedOption == -1)
					{
						saveSelectedOption = selectedOption;
					}
					break;
				}
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

	if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
	{
		saveSelectedOption = newSelectedOption;

		switch (headerMenuPosition)
		{
		case 0:
			GameOption();
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

void UIOptionsMenu::GameOption()
{
	//WINDOWS MODE
	window->SetFullscreen(true);
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
