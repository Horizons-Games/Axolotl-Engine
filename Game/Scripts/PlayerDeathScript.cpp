#include "PlayerDeathScript.h"

#ifndef ENGINE
	#include "Application.h"
	#include "ModuleScene.h"
#endif // ENGINE

#include "Components/ComponentScript.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentAnimation.h"

REGISTERCLASS(PlayerDeathScript);

PlayerDeathScript::PlayerDeathScript() : Script(), loseSceneName("00_LoseScene_VS3"), componentAnimation(nullptr)
{
	REGISTER_FIELD(loseSceneName, std::string);
}

void PlayerDeathScript::Start()
{
	componentAnimation = owner->GetComponent<ComponentAnimation>();
}

void PlayerDeathScript::ManagePlayerDeath() const
{
	DisablePlayerActions();

#ifndef ENGINE
	if (loseSceneName != "" && !componentAnimation->isPlaying() && componentAnimation->GetActualStateName() == "Death")
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + loseSceneName + ".axolotl");
	}
#endif // ENGINE
	if (!componentAnimation->isPlaying() && componentAnimation->GetActualStateName() == "Death")
	{
		LOG_VERBOSE("Player is dead");
	}
}

void PlayerDeathScript::DisablePlayerActions() const
{
	// Once the player is dead, disable its scripts
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		script->Disable();
	}

	GameObject::GameObjectView children = owner->GetChildren();

	for (const GameObject* child : children)
	{
		// In order to get the force and vertical rotation scripts to work
		// We have to disable also those scripts that are present on the player camera

		if (!child->GetComponent<ComponentCamera>())
		{
			continue;
		}

		std::vector<ComponentScript*> cameraScripts = child->GetComponents<ComponentScript>();

		for (ComponentScript* script : cameraScripts)
		{
			script->Disable();
		}
	}
}