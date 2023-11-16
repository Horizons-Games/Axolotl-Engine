#include "StdAfx.h"
#include "LightFinisherAttackScript.h"

#include "Application.h"
#include "Auxiliar/Audio/AudioData.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/LightAttackBullet.h"
#include "../Scripts/EntityDetection.h"
#include "../Scripts/PlayerAttackScript.h"

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

void LightFinisherAttackScript::ThrowStunItem(bool isMelee)
{
	ComponentAudioSource* audioSource = owner->GetComponent<ComponentAudioSource>();
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
	if (isMelee)
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::ROCKS_THROW);
		bullet->GetComponent<LightAttackBullet>()->SetImpactSound(AUDIO::SFX::PLAYER::WEAPON::ROCKS_THROW_IMPACT);
	}
	else
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::CHARGED_SHOT);
		bullet->GetComponent<LightAttackBullet>()->SetImpactSound(AUDIO::SFX::PLAYER::WEAPON::CHARGED_SHOT_IMPACT);
	}
}

bool LightFinisherAttackScript::IsAttacking()
{
	return isActivated;
}
