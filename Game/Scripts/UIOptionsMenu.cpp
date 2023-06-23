#include "UIOptionsMenu.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"



REGISTERCLASS(UIOptionsMenu);

UIOptionsMenu::UIOptionsMenu() : Script(), gameOption(nullptr), videoOption(nullptr), audioOption(nullptr),
hudOption(nullptr), keysOption(nullptr)
{
	REGISTER_FIELD(gameOption, GameObject*);
	REGISTER_FIELD(videoOption, GameObject*);
	REGISTER_FIELD(audioOption, GameObject*);
	REGISTER_FIELD(hudOption, GameObject*);
	REGISTER_FIELD(keysOption, GameObject*);
}

void UIOptionsMenu::Start()
{

}

void UIOptionsMenu::Update(float deltaTime)
{

}
