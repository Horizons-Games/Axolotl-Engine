#include "StdAfx.h"
#include "HeavyFinisherAttack.h"

#include "Application.h"

#include "Physics/Physics.h"

#include "ModuleScene.h"
#include "ModulePhysics.h"
#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(HeavyFinisherAttack);

HeavyFinisherAttack::HeavyFinisherAttack() : Script(), audioSource(nullptr), transform(nullptr), rigidBody(nullptr),
mesh(nullptr), target(nullptr), isActivated(false), isReturningToOwner(false), attackOwner(nullptr), vfx(nullptr),
returnToPlayer(false), rotateWhileAttacking(true), damage(10.0f), speed(12.0f), hitDistance(1.0f), rotationVelocity(50.0f),
loadedScene(nullptr), physics(nullptr)
{
	REGISTER_FIELD(returnToPlayer, bool);
	REGISTER_FIELD(rotateWhileAttacking, bool);
	REGISTER_FIELD(damage, float);
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(hitDistance, float);
	REGISTER_FIELD(rotationVelocity, float);
	REGISTER_FIELD(mesh, GameObject*);
}

void HeavyFinisherAttack::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	audioSource = owner->GetComponent<ComponentAudioSource>();
	vfx = owner->GetComponent<ComponentParticleSystem>();
	
	mesh->Disable();
	rigidBody->Disable();

	physics = App->GetModule<ModulePhysics>();
	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
}

void HeavyFinisherAttack::Update(float deltaTime)
{
	if (!isActivated)
	{
		return;
	}


	float3 currentPos = transform->GetGlobalPosition();
	float3 enemyPos = target->GetGlobalPosition();

	float3 vecTowardsEnemy = (enemyPos - currentPos).Normalized();

	transform->SetLocalPosition(currentPos + vecTowardsEnemy * speed * deltaTime); //Move attack
	if (rotateWhileAttacking)
	{
		transform->SetLocalRotation(Quat::RotateAxisAngle(float3::unitY, math::DegToRad(rotationVelocity * 10.0f * deltaTime))
			* transform->GetLocalRotation());
	}
	transform->UpdateTransformMatrices();

	if (currentPos.Distance(enemyPos) < hitDistance) //Attack hits target
	{
		if (!isReturningToOwner)
		{
			target->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damage);
			audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
			vfx->SetPlayAtStart(true);
			vfx->Play();

			SeekNextEnemy();
		}
		else
		{
			ResetValues();
		}
	}
}

void HeavyFinisherAttack::PerformHeavyFinisher(ComponentTransform* target, ComponentTransform* attackOwner)
{
	this->target = target;
	this->attackOwner = attackOwner;

	mesh->Enable();
	rigidBody->Disable();

	owner->SetParent(loadedScene->GetRoot());

	isReturningToOwner = false;
	isActivated = true;
}

void HeavyFinisherAttack::SeekNextEnemy()
{
	enemiesAlreadyHit.push_back(target);
	target = nullptr;

	rigidBody->Enable();
	rigidBody->UpdateRigidBody();

	std::vector<ComponentRigidBody*> collisions;
	physics->GetCollisions(rigidBody, collisions, "Enemy"); //Get enemies inside the collider

	if (collisions.empty())
	{
		return;
	}

	float3 currentPos = transform->GetGlobalPosition();

	for (ComponentRigidBody* enemy : collisions)
	{
		ComponentTransform* enemyTransform = enemy->GetOwner()->GetComponent<ComponentTransform>();

		if (!enemy->GetOwner()->GetComponent<HealthSystem>()->EntityIsAlive() //If enemy is dead
			|| (std::find(enemiesAlreadyHit.begin(), enemiesAlreadyHit.end(), enemyTransform) 
				!= enemiesAlreadyHit.end())) //If enemy already hit
		{
			continue; //next enemy
		}

		if (target == nullptr) //First enemy found
		{
			target = enemyTransform;
		}
		else if (currentPos.Distance(enemyTransform->GetGlobalPosition()) 
			< currentPos.Distance(target->GetGlobalPosition())) //Is a closer enemy than the selected
		{
			target = enemyTransform;
		}
	}

	rigidBody->Disable();

	if (target == nullptr) //No enemies to hit found
	{
		target = attackOwner;
		isReturningToOwner = true;
		if (!returnToPlayer)
		{
			ResetValues();
		}
	}
}

void HeavyFinisherAttack::ResetValues()
{
	transform->SetLocalPosition(attackOwner->GetGlobalPosition());
	transform->UpdateTransformMatrices();
	owner->SetParent(attackOwner->GetOwner());
	target = nullptr;
	attackOwner = nullptr;

	isActivated = false;
	isReturningToOwner = false;

	enemiesAlreadyHit.clear();
	enemiesInTheArea.clear();

	mesh->Disable();
}