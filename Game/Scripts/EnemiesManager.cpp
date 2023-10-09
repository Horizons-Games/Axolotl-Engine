#include "StdAfx.h"
#include "EnemiesManager.h"

#include "EnemyClass.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAnimation.h"
#include "../Scripts/RangedFastAttackBullet.h"
#include "LightAttackBullet.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Application.h"

REGISTERCLASS(EnemiesManager);

EnemiesManager::EnemiesManager() : Script(), enemiesGameObjects{}, enemyTag("Enemy"),
bulletTag("Bullet"), alluraBulletTag("AlluraBullet"), 
bulletGameObjects{}, alluraBulletGameObjects{}
{
	REGISTER_FIELD(enemyTag, std::string);
	REGISTER_FIELD(bulletTag, std::string);
}

void EnemiesManager::Start()
{
	scene = App->GetModule<ModuleScene>()->GetLoadedScene();
	enemiesGameObjects = scene->SearchGameObjectByTag(enemyTag);
}

void EnemiesManager::PauseEnemies(bool paused)
{
	bulletGameObjects = scene->SearchGameObjectByTag(bulletTag);
	alluraBulletGameObjects = scene->SearchGameObjectByTag(alluraBulletTag);

	for (int i = 0; i < enemiesGameObjects.size(); i++)
	{
		enemiesGameObjects[i]->GetComponent<EnemyClass>()->PauseEnemy(paused);
		if (paused)
		{
			enemiesGameObjects[i]->GetComponent<ComponentAnimation>()->Disable();
		}
		else
		{
			enemiesGameObjects[i]->GetComponent<ComponentAnimation>()->Enable();
		}
	}
	
	for (int i = 0; i < bulletGameObjects.size(); i++)
	{

		bulletGameObjects[i]->GetComponent<RangedFastAttackBullet>()->SetPauseBullet(paused);
		
	}
	
	for (int i = 0; i < alluraBulletGameObjects.size(); i++)
	{

		alluraBulletGameObjects[i]->GetComponent<LightAttackBullet>()->SetPauseBullet(paused);
		
	}
}
