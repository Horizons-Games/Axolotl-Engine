#include "UIGameManager.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"
#include "ModuleScene.h";


REGISTERCLASS(UIGameManager);

UIGameManager::UIGameManager() : Script(), loseStateObject(nullptr), winStateObject(nullptr), mainMenuObject(nullptr),
player(nullptr), menuIsOpen(false), hudObject(nullptr), WinSceneName("00_WinScene_VS3"), LoseSceneName("00_LoseScene_VS3")
{
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudObject, GameObject*);
	REGISTER_FIELD(setPlayer, GameObject*);
	REGISTER_FIELD(WinSceneName, std::string);
	REGISTER_FIELD(LoseSceneName, std::string);
}

void UIGameManager::Start()
{
	player = setPlayer->GetComponent<ComponentPlayer>();
}

void UIGameManager::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_M) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}
}

void UIGameManager::MenuIsOpen()
{
	if (menuIsOpen == false)
	{
		mainMenuObject->Disable();
		hudObject->Enable();
		player->SetMouse(false);
	}
	if (menuIsOpen == true)
	{
		mainMenuObject->Enable();
		hudObject->Disable();
		player->SetMouse(true);

	}
}

void UIGameManager::WinStateScene(bool setState)
{
#ifdef ENGINE

	LOG_INFO("Player wins - In game load win scene");

#else // ENGINE

	if (WinSceneName != "")
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + WinSceneName + ".axolotl");
	}

#endif // GAME
}

void UIGameManager::LoseStateScene(bool setState)
{
#ifdef ENGINE

	LOG_INFO("Player dead - In game load lose scene");

#else // ENGINE

	if (LoseSceneName != "")
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + LoseSceneName + ".axolotl");
	}

#endif // GAME
}