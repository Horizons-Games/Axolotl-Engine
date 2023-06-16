#include "EnemyManagerScript.h"

#include "GameObject/GameObject.h"

REGISTERCLASS(EnemyManagerScript);

EnemyManagerScript::EnemyManagerScript() : Script()
{
	REGISTER_FIELD(powerUp, GameObject*);
}

void EnemyManagerScript::Start()
{
}

void EnemyManagerScript::Update(float deltaTime)
{
}

GameObject* EnemyManagerScript::RequestPowerUp()
{
	//when having vectors
	//for (GameObject* powerUp : powerUps)
	//{
		if (!powerUp->IsEnabled())
		{
			return powerUp;
		}
	//}
	return nullptr;
}
