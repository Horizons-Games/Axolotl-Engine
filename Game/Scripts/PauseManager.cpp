#include "StdAfx.h"
#include "PauseManager.h"

#include "EnemyClass.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAnimation.h"

#include "PlayerManagerScript.h"
#include "RangedFastAttackBullet.h"
#include "BossChargeRockScript.h"
#include "LightAttackBullet.h"

#include "ModulePlayer.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Application.h"

REGISTERCLASS(PauseManager);

PauseManager::PauseManager() : Script(), enemiesGameObjects{}, enemyTag("Enemy"),
bulletTag("Bullet"), alluraBulletTag("AlluraBullet"), rockTag("Rock"), 
bulletGameObjects{}, alluraBulletGameObjects{}, rockGameObjects{}
{
	REGISTER_FIELD(enemyTag, std::string);
	REGISTER_FIELD(alluraBulletTag, std::string);
	REGISTER_FIELD(rockTag, std::string);
	REGISTER_FIELD(bulletTag, std::string);
}

void PauseManager::Start()
{
	scene = App->GetModule<ModuleScene>()->GetLoadedScene();
	enemiesGameObjects = scene->SearchGameObjectByTag(enemyTag);
}

void PauseManager::Pause(bool paused)
{
	bulletGameObjects = scene->SearchGameObjectByTag(bulletTag);
	alluraBulletGameObjects = scene->SearchGameObjectByTag(alluraBulletTag);
	rockGameObjects = scene->SearchGameObjectByTag(rockTag);

	PauseEnemies(paused);
	PauseBullets(paused);
	PauseRocks(paused);
	App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<PlayerManagerScript>()->FullPausePlayer(paused);
}

void PauseManager::PauseEnemies(bool paused)
{
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
}

void PauseManager::PauseBullets(bool paused)
{
	for (int i = 0; i < bulletGameObjects.size(); i++)
	{
		bulletGameObjects[i]->GetComponent<RangedFastAttackBullet>()->SetPauseBullet(paused);
	}
	
	for (int i = 0; i < alluraBulletGameObjects.size(); i++)
	{
		alluraBulletGameObjects[i]->GetComponent<LightAttackBullet>()->SetPauseBullet(paused);	
	}
}

void PauseManager::PauseRocks(bool paused)
{
	for (int i = 0; i < rockGameObjects.size(); i++)
	{
		rockGameObjects[i]->GetComponent<BossChargeRockScript>()->SetPauseRock(paused);
	}
}
