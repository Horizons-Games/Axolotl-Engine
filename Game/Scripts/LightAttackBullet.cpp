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
	velocity(25.0f),
	audioSource(nullptr),
	stunTime(10.0f),
	damageAttack(10.0f),
	defaultTargetPos(0,0,0),
	maxDistanceBullet(100.0f),
	particleSystem(nullptr), 
	particleSystemTimer(1.0f), 
	triggerParticleSystemTimer(false), 
	particleSystemCurrentTimer(0.0f),
	lifeTime(20.0f),
	parentTransform(nullptr),
	targetTransform(nullptr)
{
	REGISTER_FIELD(particleSystemTimer, float);
	REGISTER_FIELD(velocity, float);
}

void LightAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	audioSource = owner->GetComponent<ComponentAudioSource>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();
	rigidBody->SetUseRotationController(true);

	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();

	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setLinearVelocity(
		btVector3(
			forward.x,
			0,
			forward.z) * velocity);


	particleSystem = owner->GetComponent<ComponentParticleSystem>();
	particleSystem->Enable();
	particleSystemCurrentTimer = particleSystemTimer;

	playerAttackScript = owner->GetParent()->GetComponent<PlayerAttackScript>();
}

void LightAttackBullet::Update(float deltaTime)
{
	if (enemy != nullptr)
	{
		float3 targetPos = targetTransform->GetGlobalPosition();
		targetPos.y = 0.75f; 
		float3 forward = targetPos - owner->GetComponent<ComponentTransform>()->GetGlobalPosition();
		forward.Normalize();

		btRigidBody* btRb = rigidBody->GetRigidBody();
		btRb->setLinearVelocity(
			btVector3(
				forward.x,
				0,
				forward.z) * velocity);
	}

	lifeTime -= deltaTime;
	if (lifeTime <= 0.0f)
	{
		DestroyBullet();
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

void LightAttackBullet::SetStunTime(float nStunTime)
{
	stunTime = nStunTime;
}

void LightAttackBullet::SetEnemy(GameObject* nEnemy)
{
	enemy = nEnemy;
	if (enemy)
	{
		targetTransform = enemy->GetComponent<ComponentTransform>();
	}
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
