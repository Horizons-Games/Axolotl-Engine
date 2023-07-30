#include "StdAfx.h"
#include "LightAttackBullet.h"

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
	damageAttack(10.0f)
{
}

void LightAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();

	//audioSource = owner->GetComponent<ComponentAudioSource>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();
	rigidBody->SetKpForce(velocity);
}

void LightAttackBullet::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(enemy->GetComponent<ComponentTransform>()->GetGlobalPosition());
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
		DestroyBullet();
		enemy->GetComponent<EnemyClass>()->SetStunnedTime(stunTime);
		//audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); // Provisional sfx
	}	
}

void LightAttackBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
