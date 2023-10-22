#include "StdAfx.h"
#include "RangedFastAttackBehaviourScript.h"

#include "Application.h"

#include "Geometry/Ray.h"
#include "debugdraw.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleScene.h"
#include "ModuleRandom.h"
#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/RangedFastAttackBullet.h"
#include "../Scripts/AIMovement.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(RangedFastAttackBehaviourScript);

RangedFastAttackBehaviourScript::RangedFastAttackBehaviourScript() : Script(), attackCooldown(5.f), 
	lastAttackTime(0.f), particleSystemShot(nullptr), particleSystemPreShot(nullptr), audioSource(nullptr), shootPosition(nullptr),
	particleTransform(nullptr), animation(nullptr), transform(nullptr), loadedScene(nullptr), preShotDuration(0.0f),
	bulletVelocity(0.2f), bulletPrefab(nullptr), needReposition(false), newReposition(0,0,0), isPreShooting(false),
	preShootingTime(0.0f), particlePreShotTransform(nullptr), numConsecutiveShots(0.0f), minTimeConsecutiveShot(0.0f),
	maxTimeConsecutiveShot(0.0f), currentConsecutiveShots(0.0f), nextShotDuration(0.0f), shotTime(0.0f),
	isWaitingForConsecutiveShot(false), isConsecutiveShooting(false), attackDamage(10.0f), aiMovement(nullptr),
	enemyType(EnemyTypes::NONE)
{
	REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(attackDamage, float);
	REGISTER_FIELD(shootPosition, ComponentTransform*);
	REGISTER_FIELD(particleSystemShot, ComponentParticleSystem*);
	REGISTER_FIELD(particleSystemPreShot, ComponentParticleSystem*);
	REGISTER_FIELD(preShotDuration, float);
	REGISTER_FIELD(numConsecutiveShots, float);
	REGISTER_FIELD(minTimeConsecutiveShot, float);
	REGISTER_FIELD(maxTimeConsecutiveShot, float);
}

void RangedFastAttackBehaviourScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	if (particleSystemShot)
	{
		particleTransform = particleSystemShot->GetOwner()->GetComponent<ComponentTransform>();
	}
	if (particleSystemPreShot)
	{
		particlePreShotTransform = particleSystemPreShot->GetOwner()->GetComponent<ComponentTransform>();
	}
	animation = owner->GetComponent<ComponentAnimation>();
	aiMovement = owner->GetComponent<AIMovement>();
	rb = owner->GetComponent<ComponentRigidBody>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	isPreShooting = false;

	if (owner->HasComponent<EnemyClass>())
	{
		enemyType = owner->GetComponent<EnemyClass>()->GetEnemyType();
	}
}

void RangedFastAttackBehaviourScript::Update(float deltaTime)
{
	if (shootPosition)
	{
		particlePreShotTransform->SetGlobalPosition(shootPosition->GetGlobalPosition());
	}

	if (isPreShooting)
	{
		preShootingTime += deltaTime;
		if (preShootingTime >= preShotDuration)
		{
			ShootBullet();
			particleSystemPreShot->Stop();
			isPreShooting = false;
			preShootingTime = 0.0f;
		}
	}

	if (isWaitingForConsecutiveShot)
	{
		shotTime += deltaTime;
		if (shotTime >= nextShotDuration)
		{
			isPreShooting = true;
			isWaitingForConsecutiveShot = false;
			shotTime = 0.0f;
			if (particleSystemPreShot)
			{
				particleSystemPreShot->Play();
			}

			switch (enemyType)
			{
			case EnemyTypes::DRONE:
				audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_CHARGE);
				break;
			case EnemyTypes::VENOMITE:
			case EnemyTypes::MINI_BOSS:
				audioSource->PostEvent(AUDIO::SFX::NPC::VENOMITE::SHOT_CHARGE);
				break;
			}
		}
	}
}

void RangedFastAttackBehaviourScript::StartAttack()
{
	needReposition = false;
	movingToNewReposition = false;
}

void RangedFastAttackBehaviourScript::PerformAttack()
{
	isPreShooting = true;
	isConsecutiveShooting = true;
	if (particleSystemPreShot)
	{
		particleSystemPreShot->Play();
	}
}

void RangedFastAttackBehaviourScript::ShootBullet()
{
	if (particleSystemShot)
	{
		if (shootPosition)
		{
			particleTransform->SetGlobalPosition(shootPosition->GetGlobalPosition());
		}
		particleSystemShot->Play();
	}

	animation->SetParameter("IsAttacking", true);

	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);
	
	bullet->GetComponent<ComponentTransform>()->SetGlobalPosition(shootPosition->GetGlobalPosition());

	// Attach the RangedFastAttackBullet script to the new bullet to give it its logic
	ComponentScript* script = bullet->CreateComponent<ComponentScript>();
	script->SetScript(App->GetScriptFactory()->ConstructScript("RangedFastAttackBullet"));
	script->SetConstuctor("RangedFastAttackBullet");
	script->GetScript()->SetOwner(bullet);

	bullet->SetTag("Bullet");
	bullet->GetComponent<RangedFastAttackBullet>()->SetBulletVelocity(bulletVelocity);
	bullet->GetComponent<RangedFastAttackBullet>()->SetTargetTag("Player");
	bullet->GetComponent<RangedFastAttackBullet>()->SetBulletDamage(attackDamage);

	// Once the engine automatically runs the Start() for newly created objects, delete this line
	script->Start();


	switch (enemyType)
	{
	case EnemyTypes::DRONE:
		audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT);
		break;
	case EnemyTypes::VENOMITE:
	case EnemyTypes::MINI_BOSS:
		audioSource->PostEvent(AUDIO::SFX::NPC::VENOMITE::SHOT);
		break;
	}

	currentConsecutiveShots += 1.0f;
	if (currentConsecutiveShots < numConsecutiveShots)
	{
		isConsecutiveShooting = true;
		nextShotDuration = App->GetModule<ModuleRandom>()->RandomNumberInRange(minTimeConsecutiveShot, maxTimeConsecutiveShot);
		isWaitingForConsecutiveShot = true;
	}
	else
	{
		isConsecutiveShooting = false;
		currentConsecutiveShots = 0.0f;
		lastAttackTime = SDL_GetTicks() / 1000.0f;
	}
}

void RangedFastAttackBehaviourScript::Reposition(float3 nextPosition)
{
	needReposition = false;
	movingToNewReposition = true;

	aiMovement->SetTargetPosition(nextPosition);
	aiMovement->SetRotationTargetPosition(nextPosition);
}

void RangedFastAttackBehaviourScript::InterruptAttack()
{
	isPreShooting = false;
	preShootingTime = 0.0f;
	particleSystemPreShot->Stop();

	isWaitingForConsecutiveShot = false;
	isConsecutiveShooting = false;
	currentConsecutiveShots = 0.0f;
	nextShotDuration = 0.0f;
	shotTime = 0.0f;

	lastAttackTime = SDL_GetTicks() / 1000.0f - attackCooldown / 2.0f;
	if (particleSystemPreShot)
	{
		particleSystemPreShot->Stop();
	}
}

bool RangedFastAttackBehaviourScript::IsAttackAvailable() const
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown && !isPreShooting && !isConsecutiveShooting);
}

bool RangedFastAttackBehaviourScript::NeedReposition() const
{
	return needReposition;
}

bool RangedFastAttackBehaviourScript::IsPreShooting() const
{
	return isPreShooting;
}

bool RangedFastAttackBehaviourScript::IsConsecutiveShooting() const
{
	return isConsecutiveShooting;
}

bool RangedFastAttackBehaviourScript::MovingToNewReposition()
{
	if (transform->GetGlobalPosition().Equals(newReposition, 0.3f))
	{
		movingToNewReposition = false;
	}
	return movingToNewReposition;
}

void RangedFastAttackBehaviourScript::ResetScriptValues()
{
	lastAttackTime = 0.0f;
	needReposition = false;
	movingToNewReposition = false;
	newReposition = float3::zero;
}