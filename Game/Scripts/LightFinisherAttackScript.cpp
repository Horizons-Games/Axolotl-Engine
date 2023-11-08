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

#include "../Scripts/LightAttackBullet.h"
#include "../Scripts/EntityDetection.h"
#include "../Scripts/PlayerAttackScript.h"

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

void LightFinisherAttackScript::ThrowStunItem(bool isMelee)
{
	ComponentAudioSource* audioSource = owner->GetComponent<ComponentAudioSource>();
	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);

	bullet->GetComponent<LightAttackBullet>()->SetBulletVelocity(bulletVelocity);
	bullet->GetComponent<LightAttackBullet>()->SetEnemy(enemyDetection->GetEnemySelected());
	bullet->GetComponent<LightAttackBullet>()->SetStunTime(stunTime);
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
