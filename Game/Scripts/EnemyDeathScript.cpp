#include "EnemyDeathScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpLogicScript.h"

REGISTERCLASS(EnemyDeathScript);

EnemyDeathScript::EnemyDeathScript() : Script()
{
	REGISTER_FIELD(activePowerUp, GameObject*); // this should be a vector of powerUps
}

void EnemyDeathScript::ManageEnemyDeath() const
{
	GameObject* newPowerUp = RequestPowerUp();

	if (newPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogic = newPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = owner->GetComponent<ComponentTransform>();

		newPowerUpLogic->ActivatePowerUp(ownerTransform->GetPosition());
	}

	DisableEnemyActions();
}

GameObject* EnemyDeathScript::RequestPowerUp() const
{
	// when having vectors uncomment this loop
	//for (GameObject* powerUp : powerUps)
	//{
		// Make that the enemies don't always drop a powerup (20% chance)
		srand(static_cast<unsigned int>(time(0)));
		int randomActivation = rand() % 10;

		if (!activePowerUp->IsEnabled() && randomActivation < 2)
		{
			return activePowerUp;
		}
	//}

	return nullptr;
}

void EnemyDeathScript::DisableEnemyActions() const
{
	// Once the player is dead, disable its scripts
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		script->Disable();
	}
}