#include "EnemyManagerScript.h"

#include "GameObject/GameObject.h"

REGISTERCLASS(EnemyManagerScript);

EnemyManagerScript::EnemyManagerScript() : Script(), powerUp(nullptr)
{
	REGISTER_FIELD(powerUp, GameObject*); // this should be a vector of powerUps
}

void EnemyManagerScript::Start()
{
}

void EnemyManagerScript::Update(float deltaTime)
{
}

GameObject* EnemyManagerScript::RequestPowerUp()
{
	// when having vectors uncomment this loop
	//for (GameObject* powerUp : powerUps)
	//{
		if (!powerUp->IsEnabled())
		{
			return powerUp;
		}
	//}
	return nullptr;
}
