#include "StdAfx.h"
#include "LightFinisherAttackScript.h"

#include "Application.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/LightAttackBullet.h"
#include "../Scripts/EntityDetection.h"

REGISTERCLASS(LightFinisherAttackScript);

LightFinisherAttackScript::LightFinisherAttackScript() 
	: Script(), 
	loadedScene(nullptr), 
	bulletPrefab(nullptr),
	stunTime(2.0f),
	enemyDetection(nullptr),
	currentCooldown(0.0f),
	cooldown(2.0f)
{
	REGISTER_FIELD(bulletPrefab, GameObject*);
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

	bullet->Enable();
	LightAttackBullet* ligthAttackBulletScript = bullet->GetComponent<LightAttackBullet>();

	ligthAttackBulletScript->Start();
	ligthAttackBulletScript->SetInitPos(bullet->GetComponent<ComponentTransform>());
	ligthAttackBulletScript->ResetDefaultValues();
	ligthAttackBulletScript->SetEnemy(enemyDetection->GetEnemySelected());
	ligthAttackBulletScript->SetStunTime(stunTime);
	ligthAttackBulletScript->SetVelocity(50);
	ligthAttackBulletScript->SetDamage(10);
	ligthAttackBulletScript->StartMoving();
}

bool LightFinisherAttackScript::IsAttacking()
{
	return isActivated;
}
