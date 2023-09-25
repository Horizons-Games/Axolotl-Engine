#include "StdAfx.h"
#include "EnemiesManager.h"

#include "EnemyClass.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(EnemiesManager);

EnemiesManager::EnemiesManager() : Script(), enemiesGameObjects{ (nullptr), (nullptr) }
{
	REGISTER_FIELD_WITH_ACCESSORS(VectorEnemiesGameObjects, std::vector<GameObject*>);
}

void EnemiesManager::PauseEnemies(bool paused)
{
	for (int i = 0; i < enemiesGameObjects.size(); i++)
	{
		enemiesGameObjects[i]->GetComponent<EnemyClass>()->PauseEnemy(paused);
	}
}

const std::vector<GameObject*>& EnemiesManager::GetVectorEnemiesGameObjects() const
{
	return enemiesGameObjects;
}

void EnemiesManager::SetVectorEnemiesGameObjects(const std::vector<GameObject*>& enemiesGameObjects)
{
	this->enemiesGameObjects = enemiesGameObjects;
}