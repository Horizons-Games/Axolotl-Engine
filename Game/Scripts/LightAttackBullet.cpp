#include "StdAfx.h"
#include "LightAttackBullet.h"

#include "Application.h"

#include "ModuleScene.h"
#include "ModulePlayer.h"
#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentLine.h"

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
	maxLifeTimeBullet(6.0f),
	particleSystem(nullptr), 
	particleSystemTimer(1.0f), 
	triggerParticleSystemTimer(false), 
	particleSystemCurrentTimer(0.0f),
	impactSFX(nullptr),
	lifeTime(0.0f),
	parentTransform(nullptr),
	targetTransform(nullptr)
{
	REGISTER_FIELD(particleSystemTimer, float);
	REGISTER_FIELD(maxLifeTimeBullet, float);
}

void LightAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	bulletTransform = owner->GetComponent<ComponentTransform>();

	audioSource = owner->GetComponent<ComponentAudioSource>();	

	particleSystem = owner->GetComponent<ComponentParticleSystem>();
	playerAttackScript = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<PlayerAttackScript>();
}

void LightAttackBullet::Update(float deltaTime)
{
	if (isPaused)
	{
		return;
	}

	if (enemy != nullptr)
	{
		if (!enemy->IsEnabled()) 
		{
			DestroyBullet();
			return;
		}

		float3 targetPos = targetTransform->GetGlobalPosition();
		targetPos.y += 1; 
		float3 forward = targetPos - owner->GetComponent<ComponentTransform>()->GetGlobalPosition();
		forward.Normalize();

		btRigidBody* btRb = rigidBody->GetRigidBody();
		btRb->setLinearVelocity(
			btVector3(
				forward.x,
				0,
				forward.z) * velocity);
	}

	lifeTime += deltaTime;
	if (lifeTime > maxLifeTimeBullet)
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

		if (particleSystem)
		{
			particleSystem->Stop();
		}
		DestroyBullet();
	}
}

void LightAttackBullet::StartMoving()
{
	rigidBody->Enable();

	RepositionBullet();

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

}

void LightAttackBullet::SetPauseBullet(bool isPaused)
{
	this->isPaused = isPaused;
	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();
	btRigidBody* btRb = rigidBody->GetRigidBody();

	if (isPaused)
	{
		btRb->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
	}
	else
	{
		btRb->setLinearVelocity(
			btVector3(
				forward.x,
				0,
				forward.z) * velocity);
	}
}

//Function to reposition the bullet to the front of the player before shooting
void LightAttackBullet::RepositionBullet()
{
	float3 currentForward = bulletTransform->GetGlobalForward().Normalized();
	float3 desiredForward;
	float3 height = float3(0.0f, 1.0f, 0.0f);
	// Create a new bullet
	if (enemy)
	{
		desiredForward = (targetTransform->
			GetGlobalPosition() - parentTransform->GetGlobalPosition()).Normalized();

		bulletTransform->SetGlobalPosition(parentTransform->GetGlobalPosition() + height
			+ (desiredForward).Normalized());

		float angle = Quat::FromEulerXYZ(currentForward.x, currentForward.y, currentForward.z).
			AngleBetween(Quat::FromEulerXYZ(desiredForward.x, desiredForward.y, desiredForward.z));
		angle = math::Abs(math::RadToDeg(angle));

		bulletTransform->SetGlobalRotation(bulletTransform->GetGlobalRotation().LookAt(currentForward, desiredForward,
			float3::unitZ, float3::unitY));
	}
	else
	{
		desiredForward = parentTransform->GetGlobalForward();
		bulletTransform->SetGlobalPosition(parentTransform->GetGlobalPosition() + height
			+ (desiredForward).Normalized());

		float angle = Quat::FromEulerXYZ(currentForward.x, currentForward.y, currentForward.z).
			AngleBetween(Quat::FromEulerXYZ(desiredForward.x, desiredForward.y, desiredForward.z));
		angle = math::Abs(math::RadToDeg(angle));

		bulletTransform->SetGlobalRotation(bulletTransform->GetGlobalRotation().LookAt(currentForward, desiredForward,
			float3::unitZ, float3::unitY));
	}

	bulletTransform->RecalculateLocalMatrix();
	bulletTransform->UpdateTransformMatrices();
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

void LightAttackBullet::SetImpactSound(const wchar_t* sound)
{
	impactSFX = sound;
}

void LightAttackBullet::SetVelocity(float nVelocity)
{
	velocity = nVelocity;
}

void LightAttackBullet::SetInitPos(ComponentTransform* nInitPos)
{
	initPos = nInitPos;
}

void LightAttackBullet::ResetDefaultValues()
{
	lifeTime = 0.0f;

	bulletTransform->SetGlobalPosition(initPos->GetGlobalPosition());
	bulletTransform->SetGlobalRotation(initPos->GetGlobalRotation());
	bulletTransform->RecalculateLocalMatrix();

	rigidBody->UpdateRigidBody();
	rigidBody->Enable();
	rigidBody->SetDefaultPosition();
	rigidBody->SetUseRotationController(true);

	if (particleSystem)
	{
		particleSystem->Enable();
		particleSystemCurrentTimer = particleSystemTimer;
	}

	owner->GetComponent<ComponentMeshRenderer>()->Enable();
}

void LightAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	


	if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("PriorityTarget"))
	{
		if (impactSFX != nullptr)
		{
			audioSource->PostEvent(impactSFX);
		}

		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damageAttack);
		other->GetOwner()->GetComponent<EnemyClass>()->SetStunnedTime(stunTime);

		// Disable the visuals and the rigidbody while the particles are being played
		//rigidBody->SetIsTrigger(true);
		
		owner->GetComponent<ComponentMeshRenderer>()->Disable();

		if (owner->HasComponent<ComponentLine>())
		{
			owner->GetComponent<ComponentLine>()->Disable();
		}

		if (particleSystem)
		{
			particleSystem->Play();
		}
		triggerParticleSystemTimer = true;
	}

	else if (!other->IsTrigger() && !other->GetOwner()->CompareTag("Player"))
	{
		if (impactSFX != nullptr)
		{
			audioSource->PostEvent(impactSFX);
		}
		owner->Disable();
	}
}

void LightAttackBullet::DestroyBullet()
{
	if (particleSystem)
	{
		particleSystem->Disable();
	}
	owner->Disable();
}

