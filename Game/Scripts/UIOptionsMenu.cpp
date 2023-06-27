#include "StdAfx.h"
#include "UIOptionsMenu.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"


REGISTERCLASS(UIOptionsMenu);

UIOptionsMenu::UIOptionsMenu() : Script(), gameOptionButton(nullptr), videoOptionButton(nullptr), audioOptionButton(nullptr),
hudOptionButton(nullptr), keysOptionButton(nullptr), gameOptionCanvas(nullptr), videoOptionCanvas(nullptr), audioOptionCanvas(nullptr),
hudOptionCanvas(nullptr), keysOptionCanvas(nullptr), gameOptionHover(nullptr), videoOptionHover(nullptr), audioOptionHover(nullptr),
hudOptionHover(nullptr), keysOptionHover(nullptr)
{
	REGISTER_FIELD(gameOptionButton, GameObject*);
	REGISTER_FIELD(videoOptionButton, GameObject*);
	//REGISTER_FIELD(audioOptionButton, GameObject*);
	REGISTER_FIELD(hudOptionButton, GameObject*);
	REGISTER_FIELD(keysOptionButton, GameObject*);

	REGISTER_FIELD(gameOptionCanvas, GameObject*);
	REGISTER_FIELD(videoOptionCanvas, GameObject*);
	//REGISTER_FIELD(audioOptionCanvas, GameObject*);
	REGISTER_FIELD(hudOptionCanvas, GameObject*);
	REGISTER_FIELD(keysOptionCanvas, GameObject*);

	REGISTER_FIELD(gameOptionHover, GameObject*);
	REGISTER_FIELD(videoOptionHover, GameObject*);
	//REGISTER_FIELD(audioOptionHover, GameObject*);
	REGISTER_FIELD(hudOptionHover, GameObject*);
	REGISTER_FIELD(keysOptionHover, GameObject*);
}

void UIOptionsMenu::Start()
{
	gameOptionComponentButton = gameOptionButton->GetComponent<ComponentButton>();
	videoOptionComponentButton = videoOptionButton->GetComponent<ComponentButton>();
	//audioOptionComponentButton = audioOptionButton->GetComponent<ComponentButton>();
	hudOptionComponentButton = hudOptionButton->GetComponent<ComponentButton>();
	keysOptionComponentButton = keysOptionButton->GetComponent<ComponentButton>();

	buttonsAndCanvas.push_back(OptionsButtonInfo{ gameOptionComponentButton, gameOptionCanvas, gameOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ videoOptionComponentButton, videoOptionCanvas, videoOptionHover });
	//buttonsAndCanvas.push_back(OptionsButtonInfo{ audioOptionComponentButton, audioOptionCanvas, audioOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ hudOptionComponentButton, hudOptionCanvas, hudOptionHover });
	buttonsAndCanvas.push_back(OptionsButtonInfo{ keysOptionComponentButton, keysOptionCanvas, keysOptionHover });
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
