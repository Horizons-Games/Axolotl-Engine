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
	currentButtonIndex = App->GetModule<ModuleUI>();

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
	input = App->GetModule<ModuleInput>();
	ControlEnable();
}

void UIOptionsMenu::ControlEnable()
{
	if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN)
	{
		if ( input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN)
		{
			if (selectedPositon >= 1)
			{
				newSelectedPositon--;
			}
		}
		else if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
		{
			if (selectedPositon < 3)
			{
				newSelectedPositon++;
			}
		}

		//ENABLE-DISABLE THE MAIN BAR MENU OPTION HOVER - GAME - VIDEO - AUDIO - CONTROLS
		buttonsAndCanvas[selectedPositon].canvas->Disable();
		buttonsAndCanvas[selectedPositon].hovered->Disable();
		selectedPositon = newSelectedPositon;
		buttonsAndCanvas[newSelectedPositon].canvas->Enable();
		buttonsAndCanvas[newSelectedPositon].hovered->Enable();
	}

	if (input->GetKey(SDL_SCANCODE_LEFT) == KeyState::DOWN || input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::DOWN ||
		input->GetLeftJoystickDirection().horizontalDirection == JoystickHorizontalDirection::RIGHT || 
		input->GetLeftJoystickDirection().horizontalDirection == JoystickHorizontalDirection::LEFT)
	{
		//LOOK FOR THE ACTUAL SELECTED BUTTON
		for (int a = 0; a >= buttonsAndCanvas[selectedPositon].canvas->GetChildren().size(); a++)
		{
			if (buttonsAndCanvas[selectedPositon].canvas->GetChildren()[a]->GetComponent<ComponentButton>()->IsHovered())
			{
				actualButtonHover = a;

				//LOOK FOR THE ACTUAL OPTION ENABLE
				for (int b = 0; b >= buttonsAndCanvas[selectedPositon].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren().size(); b++)
				{
					if (buttonsAndCanvas[selectedPositon].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren()[b]->IsEnabled())
					{
						selectedOption = b;
						break;
					}
				}
			}
		}
		//.ACTUAL CANVAS -> HOVERED BUTTON -> ALWAYS SECOND CHILDREN -> SELECTED OPTION
		buttonsAndCanvas[selectedPositon].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren()[selectedOption]->Disable();
		selectedOption++;
		buttonsAndCanvas[selectedPositon].canvas->GetChildren()[actualButtonHover]->GetChildren()[1]->GetChildren()[selectedOption]->Enable();

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
		if (button->IsHovered() && selectedPositon != i)
		{
			buttonsAndCanvas[i].hovered->Enable();
		}
		else
		{
			buttonsAndCanvas[i].hovered->Disable();
		}
		if (button->IsClicked())
		{
			newSelectedPositon = i;
		}
	}
	if (newSelectedPositon != -1 && newSelectedPositon != selectedPositon)
	{
		buttonsAndCanvas[selectedPositon].canvas->Disable();
		buttonsAndCanvas[selectedPositon].button->Enable();
		selectedPositon = newSelectedPositon;
		buttonsAndCanvas[selectedPositon].canvas->Enable();
		buttonsAndCanvas[selectedPositon].button->Disable();
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
