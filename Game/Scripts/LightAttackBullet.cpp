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
	defaultTargetPos.y = 0;
	defaultTargetPos.Normalize();
	defaultTargetPos = defaultTargetPos * maxDistanceBullet;
	defaultTargetPos += parentTransform->GetGlobalPosition();	

	particleSystem = owner->GetComponent<ComponentParticleSystem>();

	if (particleSystem)
	{
		particleSystem->Enable();
		particleSystemCurrentTimer = particleSystemTimer;
	}

	playerAttackScript = owner->GetParent()->GetComponent<PlayerAttackScript>();
}

void LightAttackBullet::Update(float deltaTime)
{
	if (isPaused)
	{
		return;
	}
	float angleInDegrees = 20.0f;
	Quat rotationZ = Quat::RotateZ(DegToRad(angleInDegrees));

	// Obtiene la rotación actual del objeto
	Quat currentRotation = GetOwner()->GetComponent<ComponentTransform>()->GetLocalRotation();

	// Combina la rotación actual con la nueva rotación alrededor del eje Z
	Quat newRotation = currentRotation * rotationZ;

	// Establece la nueva rotación para el objeto
	GetOwner()->GetComponent<ComponentTransform>()->SetLocalRotation(newRotation);
	LOG_DEBUG("Rotation Z: {},{},{}", currentRotation.x, currentRotation.y, currentRotation.z);
	if (enemy != nullptr)
	{
		defaultTargetPos = enemy->GetComponent<ComponentTransform>()->GetGlobalPosition();
		defaultTargetPos.y += 1;
		rigidBody->SetPositionTarget(defaultTargetPos);
	}

	else
	{
		//rigidBody->SetKpForce(2.0f);

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

		if (particleSystem)
		{
			particleSystem->Stop();
		}

		DestroyBullet();
	}
}

void LightAttackBullet::SetPauseBullet(bool isPaused)
{
	this->isPaused = isPaused;
	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();
	if (isPaused)
	{
		rigidBody->SetKpForce(0);
	}
	else
	{
		rigidBody->SetKpForce(velocity);
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
	if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("PriorityTarget"))
	{
		if (playerAttackScript->IsMelee())
		{
			audioSource->PostEvent(AUDIO::SFX::NPC::SHOT_IMPACT); // Provisional sfx
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

		if (particleSystem)
		{
			particleSystem->Play();
		}
		triggerParticleSystemTimer = true;
	}

	else if (!other->IsTrigger() && !other->GetOwner()->CompareTag("Player"))
	{
		if (playerAttackScript->IsMelee())
		{
			audioSource->PostEvent(AUDIO::SFX::NPC::SHOT_IMPACT); // Provisional sfx
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
