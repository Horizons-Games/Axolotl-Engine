#include "StdAfx.h"
#include "EnemiesManager.h"

#include "EnemyClass.h"
#include "Components/ComponentScript.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Application.h"

REGISTERCLASS(EnemiesManager);

EnemiesManager::EnemiesManager() : Script(), enemiesGameObjects{}, tag("Enemy")
{
	REGISTER_FIELD(tag, std::string);
}

void EnemiesManager::Start()
{
	Scene* scene = App->GetModule<ModuleScene>()->GetLoadedScene();
	enemiesGameObjects = scene->SearchGameObjectByTag(tag);

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