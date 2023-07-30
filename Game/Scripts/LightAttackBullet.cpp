#include "StdAfx.h"
#include "LightAttackBullet.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"

#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"

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
}

void LightAttackBullet::Update(float deltaTime)
{
}

void LightAttackBullet::SetBulletVelocity(float velocity)
{
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

	audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); // Provisional sfx
	DestroyBullet();
}

void LightAttackBullet::DestroyBullet()
{
}
