#include "MeleeHeavyAttackBehaviourScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyDeathScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/AIMovement.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(MeleeHeavyAttackBehaviourScript);

MeleeHeavyAttackBehaviourScript::MeleeHeavyAttackBehaviourScript() : Script(), aiMovement(nullptr),
	attackState(ExplosionState::NOTDEAD), targetPlayer(nullptr), explosionDamage(30.0f),
	explosionTime(3.0f), parentDeathScript(nullptr)
{
	REGISTER_FIELD(explosionDamage, float);
	REGISTER_FIELD(explosionTime, float);
}

void MeleeHeavyAttackBehaviourScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	parentDeathScript = owner->GetParent()->GetComponent<EnemyDeathScript>();
	componentAudioSource = owner->GetParent()->GetComponent<ComponentAudioSource>();
	aiMovement = owner->GetParent()->GetComponent<AIMovement>();
	rigidBody->SetKpForce(50);
}

void MeleeHeavyAttackBehaviourScript::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());

	if (attackState == ExplosionState::WAITING_EXPLOSION)
	{
		explosionTime -= deltaTime;
		UpdateDroneColor();

		if (explosionTime < 0)
		{
			attackState = ExplosionState::EXPLODING;
		}
	}

	else if (attackState == ExplosionState::EXPLODING)
	{
		if (targetPlayer)
		{
			targetPlayer->GetComponent<HealthSystem>()->TakeDamage(explosionDamage);
		}

		owner->GetParent()->GetComponent<ComponentRigidBody>()->SetKpForce(0.5f);
		attackState = ExplosionState::DEAD;
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_TIMER);
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::EXPLOSION);
		parentDeathScript->ManageEnemyDeath();
	}
}

void MeleeHeavyAttackBehaviourScript::UpdateDroneColor()
{
	//When we have tint the color of the drone will change.
}

void MeleeHeavyAttackBehaviourScript::TriggerExplosion()
{
	SeekBehaviourScript* enemySeekBehaviour = owner->GetParent()->GetComponent<SeekBehaviourScript>();
	float3 targetPos = enemySeekBehaviour->GetTarget()->GetComponent<ComponentTransform>()->GetGlobalPosition();

	componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::TIMER);
	attackState = ExplosionState::WAITING_EXPLOSION;
}

ExplosionState MeleeHeavyAttackBehaviourScript::HasExploded() const
{
	return attackState;
}

void MeleeHeavyAttackBehaviourScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		targetPlayer = other->GetOwner();
	}
}

void MeleeHeavyAttackBehaviourScript::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		targetPlayer = nullptr;
	}
}