#include "StdAfx.h"
#include "LightAttackBullet.h"

#include "Application.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyClass.h"

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
	maxDistanceBullet(10.0f)
{
}

void LightAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	//audioSource = owner->GetComponent<ComponentAudioSource>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();
	rigidBody->SetUseRotationController(true);

	defaultTargetPos = parentTransform->GetGlobalForward();
	defaultTargetPos.Normalize();
	defaultTargetPos = defaultTargetPos * maxDistanceBullet;
	defaultTargetPos += parentTransform->GetGlobalPosition();	
	defaultTargetPos.y = 0;
}

void LightAttackBullet::Update(float deltaTime)
{
	rigidBody->SetKpForce(2.0f);

	if (enemy != nullptr)
	{
		rigidBody->SetPositionTarget(enemy->GetComponent<ComponentTransform>()->GetGlobalPosition());
	}

	else
	{
		defaultTargetPos.y -= 0.1f;
		rigidBody->SetPositionTarget(defaultTargetPos);
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

void LightAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Enemy"))
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damageAttack);
		other->GetOwner()->GetComponent<EnemyClass>()->SetStunnedTime(stunTime);
		DestroyBullet();
		//audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); // Provisional sfx
	}

	else if (!other->IsTrigger() && !other->GetOwner()->CompareTag("Player"))
	{
		DestroyBullet();
	}
}

void LightAttackBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
