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
padTriggersIMG (nullptr)
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

	Assert(gameOptionButton != nullptr, axo::Format("Script owned by {} is fucked!!", owner));

	gameOptionComponentButton = gameOptionButton->GetComponent<ComponentButton>();
	videoOptionComponentButton = videoOptionButton->GetComponent<ComponentButton>();
	audioOptionComponentButton = audioOptionButton->GetComponent<ComponentButton>();
	controlOptionComponentButton = controlOptionButton->GetComponent<ComponentButton>();

	buttonsAndCanvas.push_back(OptionsButtonInfo{ gameOptionComponentButton, gameOptionCanvas, gameOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ videoOptionComponentButton, videoOptionCanvas, videoOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ audioOptionComponentButton, audioOptionCanvas, audioOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ controlOptionComponentButton, controlOptionCanvas, controlOptionHover });
	gameOptionComponentButton->Disable();
	videoOptionComponentButton->Disable();
	audioOptionComponentButton->Disable();
	controlOptionComponentButton->Disable();
	buttonsAndCanvas[0].canvas->Enable();
	buttonsAndCanvas[0].hovered->Enable();
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
		if ( input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN)
		{
			if (headerMenuPosition >= 1)
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

		//ENABLE-DISABLE THE MAIN BAR MENU OPTION HOVER - GAME - VIDEO - AUDIO - CONTROLS
		buttonsAndCanvas[headerMenuPosition].canvas->Disable();
		buttonsAndCanvas[headerMenuPosition].hovered->Disable();
		headerMenuPosition = newHeaderMenuPosition;
		buttonsAndCanvas[newHeaderMenuPosition].canvas->Enable();
		buttonsAndCanvas[newHeaderMenuPosition].hovered->Enable();
	}

	// MOVE LEFT OR RIGHT THE OPTIONS
	if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN ||
		input->GetLeftJoystickDirection().horizontalDirection == JoystickHorizontalDirection::RIGHT ||
		input->GetLeftJoystickDirection().horizontalDirection == JoystickHorizontalDirection::LEFT)
	{
		//LOOK FOR THE current SELECTED BUTTON
		if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren().size() != 0)
		{
			for (int a = 0; a </*>=*/ buttonsAndCanvas[headerMenuPosition].canvas->GetChildren().size(); a++)
			{
				if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[a]->GetComponent<ComponentButton>()->IsHovered())
				{
					actualButtonHover = a;
					break;
				}
			}
			//LOOK FOR THE ACTUAL OPTION ENABLE
			if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren().size() != 0)
			{
				for (int b = 0;
					b >= buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren().size(); b++)
				{
					if (buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren()[b]->IsEnabled())
					{
						selectedOption = b;
						break;
					}
				}
			}
			// DISABLE PREVIUS OPTION
			buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren()[selectedOption]->Disable();

			if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN ||
				input->GetLeftJoystickDirection().horizontalDirection == JoystickHorizontalDirection::LEFT)
			{
				selectedOption--;
			}
			else if (input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN ||
				input->GetLeftJoystickDirection().horizontalDirection == JoystickHorizontalDirection::RIGHT)
			{
				selectedOption++;
			}
			//.ACTUAL CANVAS -> HOVERED BUTTON -> ALWAYS SECOND CHILDREN -> SELECTED OPTION
			buttonsAndCanvas[headerMenuPosition].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren()[selectedOption]->Enable();
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
