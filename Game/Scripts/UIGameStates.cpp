#include "UIGameStates.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"
#include "ModuleScene.h";


REGISTERCLASS(UIGameStates);

UIGameStates::UIGameStates() : Script(), loseStateObject(nullptr), winStateObject(nullptr), mainMenuObject(nullptr),
player(nullptr), menuIsOpen(false), hudStateObject(nullptr), WinSceneName("00_WinScene_VS3"), LoseSceneName("00_LoseScene_VS3")
{
	//REGISTER_FIELD(winStateObject, GameObject*);
	//REGISTER_FIELD(loseStateObject, GameObject*);
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudStateObject, GameObject*);
	REGISTER_FIELD(setPlayer, GameObject*);
	REGISTER_FIELD(WinSceneName, std::string);
	REGISTER_FIELD(LoseSceneName, std::string);
}

void UIGameStates::Start()
{
	player = static_cast<ComponentPlayer*>(setPlayer->GetComponent(ComponentType::PLAYER));
}

void UIGameStates::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_M) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}
}

void UIGameStates::MenuIsOpen()
{
	if (menuIsOpen == false)
	{
		mainMenuObject->Disable();
		hudStateObject->Enable();
		player->SetMouse(false);
	}
	if (menuIsOpen == true)
	{
		mainMenuObject->Enable();
		hudStateObject->Disable();
		player->SetMouse(true);

	}
}

void UIGameStates::WinStateScene(bool setState)
{
#ifndef ENGINE
	if (WinSceneName != "")
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + WinSceneName + ".axolotl");
	}
#endif // ENGINE
	if (WinSceneName != "")
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + WinSceneName + ".axolotl");
	}

	/*
	if (setState == true)
	{
		winStateObject->Enable();
		player->SetMouse(true);
	}
	else
	{
		winStateObject->Disable();
		player->SetMouse(false);
	}
	*/
}

void UIGameStates::LoseStateScene(bool setState)
{
#ifndef ENGINE
	if (LoseSceneName != "")
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + LoseSceneName + ".axolotl");
	}
#endif // ENGINE
	/*
	if (setState == true)
	{
		loseStateObject->Enable();
		player->SetMouse(true);
	}
	else
	{
		loseStateObject->Disable();
		player->SetMouse(false);
	}
	*/
}