#include "RangedFastAttackBullet.h"

#include "Application.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Scripting/ScriptFactory.h"

#include "Physics/Physics.h"
#include "Geometry/Ray.h"
#include "debugdraw.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentMeshRenderer.h"

#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(RangedFastAttackBullet);


RangedFastAttackBullet::RangedFastAttackBullet() : Script(), parentTransform(nullptr), rigidBody(nullptr), velocity(15.0f), 
audioSource(nullptr), bulletLifeTime(10.0f), damageAttack(10.0f), rayAttackSize(100.0f), originTime(0.0f), 
particleSystem(nullptr), waitParticlesToDestroy(false), particlesDuration(1.0f), mesh(nullptr), targetTag("Not Selected")
{
}

void RangedFastAttackBullet::Start()
{
	InitializeBullet();
}

void RangedFastAttackBullet::Update(float deltaTime)
{
	if (waitParticlesToDestroy)
	{
		particlesDuration -= deltaTime;
	}

	if (SDL_GetTicks() / 1000.0f > originTime + bulletLifeTime || particlesDuration <= 0.0f)
	{
		DestroyBullet();
	}
}

void RangedFastAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	if (waitParticlesToDestroy || other->IsTrigger())
	{
		return;
	}

	if (other->GetOwner()->CompareTag(targetTag))
	{
		HealthSystem* playerHealthScript = other->GetOwner()->GetComponent<HealthSystem>();
		playerHealthScript->TakeDamage(damageAttack);
	}

	audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT); //Provisional sfx

	mesh->Disable();
	rigidBody->Disable();
	if (particleSystem)
	{
		particleSystem->Play();
	}
	waitParticlesToDestroy = true;

}

void RangedFastAttackBullet::InitializeBullet()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	audioSource = owner->GetComponent<ComponentAudioSource>();
	particleSystem = GetOwner()->GetComponent<ComponentParticleSystem>();
	mesh = GetOwner()->GetComponent<ComponentMeshRenderer>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();
	rigidBody->SetDrawCollider(false);

	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();

	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setLinearVelocity(
		btVector3(
			forward.x,
			0,
			forward.z) * velocity);

	originTime = SDL_GetTicks() / 1000.0f;
}

void RangedFastAttackBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}

void RangedFastAttackBullet::SetPauseBullet(bool isPaused)
{
	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();
	if (isPaused)
	{
		rigidBody->GetRigidBody()->setLinearVelocity(
			btVector3(
				forward.x,
				0,
				forward.z) * 0);
	}
	else
	{
		rigidBody->GetRigidBody()->setLinearVelocity(
			btVector3(
				forward.x,
				0,
				forward.z) * velocity);
	}
}

void RangedFastAttackBullet::SetBulletVelocity(float nVelocity)
{
	velocity = nVelocity;
}

void RangedFastAttackBullet::SetTargetTag(std::string nTag)
{
	targetTag = nTag;
}

void RangedFastAttackBullet::SetBulletDamage(float damage)
{
	damageAttack = damage;
}
