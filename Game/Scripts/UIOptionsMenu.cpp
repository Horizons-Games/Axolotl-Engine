#include "StdAfx.h"
#include "UIOptionsMenu.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"


REGISTERCLASS(UIOptionsMenu);

UIOptionsMenu::UIOptionsMenu() : Script(), gameOptionButton(nullptr), videoOptionButton(nullptr), audioOptionButton(nullptr),
controlOptionButton(nullptr), gameOptionCanvas(nullptr), videoOptionCanvas(nullptr), audioOptionCanvas(nullptr), 
gameOptionHover(nullptr), videoOptionHover(nullptr), audioOptionHover(nullptr), controlOptionHover(nullptr)
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
}

void UIOptionsMenu::Start()
{
	gameOptionComponentButton = gameOptionButton->GetComponent<ComponentButton>();
	videoOptionComponentButton = videoOptionButton->GetComponent<ComponentButton>();
	audioOptionComponentButton = audioOptionButton->GetComponent<ComponentButton>();
	controlOptionComponentButton = controlOptionButton->GetComponent<ComponentButton>();

	buttonsAndCanvas.push_back(OptionsButtonInfo{ gameOptionComponentButton, gameOptionCanvas, gameOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ videoOptionComponentButton, videoOptionCanvas, videoOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ audioOptionComponentButton, audioOptionCanvas, audioOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ controlOptionComponentButton, controlOptionCanvas, controlOptionHover });
	gameOptionComponentButton->Disable();
}

void UIOptionsMenu::Update(float deltaTime)
{
	int newSelectedPositon = -1;

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
