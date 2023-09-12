#include "StdAfx.h"
#include "LightAttackBullet.h"

#include "Application.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentMeshRenderer.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyClass.h"
#include "../Scripts/PlayerAttackScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(LightAttackBullet);

LightAttackBullet::LightAttackBullet() :
	Script(),
	enemy(nullptr),
	velocity(15.0f),
	audioSource(nullptr),
	stunTime(10.0f),
	damageAttack(10.0f),
	defaultTargetPos(0,0,0),
	maxDistanceBullet(10.0f),
	particleSystem(nullptr), 
	particleSystemTimer(1.0f), 
	triggerParticleSystemTimer(false), 
	particleSystemCurrentTimer(0.0f)
{
	REGISTER_FIELD(particleSystemTimer, float);
	REGISTER_FIELD(maxDistanceBullet, float);
}

void LightAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	audioSource = owner->GetComponent<ComponentAudioSource>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();
	rigidBody->SetUseRotationController(true);

	defaultTargetPos = parentTransform->GetGlobalForward();
	defaultTargetPos.Normalize();
	defaultTargetPos = defaultTargetPos * maxDistanceBullet;
	defaultTargetPos += parentTransform->GetGlobalPosition();	
	defaultTargetPos.y = 0;

	particleSystem = owner->GetComponent<ComponentParticleSystem>();
	particleSystem->Enable();
	particleSystemCurrentTimer = particleSystemTimer;

	playerAttackScript = owner->GetParent()->GetComponent<PlayerAttackScript>();
}

void LightAttackBullet::Update(float deltaTime)
{
	if (enemy != nullptr)
	{
		rigidBody->SetPositionTarget(enemy->GetComponent<ComponentTransform>()->GetGlobalPosition());
	}

	else
	{
		defaultTargetPos.y -= 0.1f;
		rigidBody->SetKpForce(2.0f);

		rigidBody->SetPositionTarget(defaultTargetPos);
	}

	if (!triggerParticleSystemTimer)
	{
		return;
	}

	// When the particles are ready to be played, play them and after them, delete the bullet
	particleSystemCurrentTimer -= deltaTime;
	if (particleSystemCurrentTimer <= 0.0f)
	{
		particleSystemCurrentTimer = particleSystemTimer;
		triggerParticleSystemTimer = false;
		particleSystem->Stop();

		DestroyBullet();
	}
}

void LightAttackBullet::SetBulletVelocity(float nVelocity)
{
	velocity = nVelocity;
	rigidBody->SetKpForce(velocity);
}

void LightAttackBullet::SetStunTime(float nStunTime)
{
	stunTime = nStunTime;
}

void LightAttackBullet::SetEnemy(GameObject* nEnemy)
{
	enemy = nEnemy;
}

void LightAttackBullet::SetDamage(float nDamageAttack)
{
	damageAttack = nDamageAttack;
}

void LightAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Enemy"))
	{
		if (playerAttackScript->IsMeleeAvailable())
		{
			audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); // Provisional sfx
		}
		else
		{
			audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::ELECTRIC_SHOT); // Provisional sfx
		}
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damageAttack);
		other->GetOwner()->GetComponent<EnemyClass>()->SetStunnedTime(stunTime);

		// Disable the visuals and the rigidbody while the particles are being played
		rigidBody->SetIsTrigger(true);
		owner->GetComponent<ComponentMeshRenderer>()->Disable();
		particleSystem->Play();
		triggerParticleSystemTimer = true;
	}

	else if (!other->IsTrigger() && !other->GetOwner()->CompareTag("Player"))
	{
		if (playerAttackScript->IsMeleeAvailable())
		{
			audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); // Provisional sfx
		}
		else
		{
			audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::ELECTRIC_SHOT); // Provisional sfx
		}
		DestroyBullet();
	}
}

void LightAttackBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveParticleSystem(particleSystem);
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
