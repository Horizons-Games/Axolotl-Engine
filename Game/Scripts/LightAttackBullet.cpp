#include "StdAfx.h"
#include "LightAttackBullet.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(LightAttackBullet);

LightAttackBullet::LightAttackBullet() :
	Script(),
	enemy(nullptr),
	velocity(15.0f),
	audioSource(nullptr),
	stunTime(10.0f),
	damageAttack(10.0f)
{
}

void LightAttackBullet::Start()
{
	ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();

	//enemyTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	//audioSource = owner->GetComponent<ComponentAudioSource>();

	rigidBody->Enable();
	rigidBody->SetPositionTarget(owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalPosition());
	rigidBody->SetKpForce(velocity);
}

void LightAttackBullet::SetBulletVelocity(float nVelocity)
{
	velocity = nVelocity;

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
	if (other->IsTrigger())
	{
		return;
	}

	if (other->GetOwner() == enemy)
	{
		HealthSystem* playerHealthScript = enemy->GetComponent<HealthSystem>();
		playerHealthScript->TakeDamage(damageAttack);
	}

	//audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); // Provisional sfx
	DestroyBullet();
}

void LightAttackBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
