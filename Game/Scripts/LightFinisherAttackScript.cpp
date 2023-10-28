#include "StdAfx.h"
#include "LightFinisherAttackScript.h"

#include "Application.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"

#include "../Scripts/LightAttackBullet.h"
#include "../Scripts/EntityDetection.h"

REGISTERCLASS(LightFinisherAttackScript);

LightFinisherAttackScript::LightFinisherAttackScript() 
	: Script(), 
	loadedScene(nullptr), 
	bulletPrefab(nullptr),
	bulletVelocity(15.0f),
	stunTime(2.0f),
	enemyDetection(nullptr),
	currentCooldown(0.0f),
	cooldown(2.0f)
{
	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(stunTime, float);
	REGISTER_FIELD(enemyDetection, EntityDetection*);
}

void LightFinisherAttackScript::Start()
{
	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
}

void LightFinisherAttackScript::ThrowStunItem()
{
	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);

	bullet->GetComponent<LightAttackBullet>()->SetBulletVelocity(bulletVelocity);
	bullet->GetComponent<LightAttackBullet>()->SetEnemy(enemyDetection->GetEnemySelected());
	bullet->GetComponent<LightAttackBullet>()->SetStunTime(stunTime);
}

bool LightFinisherAttackScript::IsAttacking()
{
	return isActivated;
}
