#include "EnemyDeathScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/EnemyManagerScript.h"
#include "../Scripts/PowerUpLogicScript.h"

REGISTERCLASS(EnemyDeathScript);

EnemyDeathScript::EnemyDeathScript() : Script()
{
}

void EnemyDeathScript::ManageEnemyDeath() const
{
	EnemyManagerScript* enemyManager = owner->GetComponent<EnemyManagerScript>();

	GameObject* newPowerUp = enemyManager->RequestPowerUp();
	if (newPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogicScript = newPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = owner->GetComponent<ComponentTransform>();
		newPowerUpLogicScript->ActivatePowerUp(ownerTransform->GetPosition());
	}
}