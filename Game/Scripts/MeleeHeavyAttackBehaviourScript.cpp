#include "MeleeHeavyAttackBehaviourScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyDroneScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(MeleeHeavyAttackBehaviourScript);

MeleeHeavyAttackBehaviourScript::MeleeHeavyAttackBehaviourScript() : Script(),
	attackState(ExplosionState::NOTDEAD), targetPlayer(nullptr), explosionDamage(30.0f),
	explosionTime(3.0f)
{
	REGISTER_FIELD(explosionDamage, float);
	REGISTER_FIELD(explosionTime, float);
}

void MeleeHeavyAttackBehaviourScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	parentHealthSystem = owner->GetParent()->GetComponent<HealthSystem>();
	parentEnemyDroneScript = owner->GetParent()->GetComponent<EnemyDroneScript>();
	componentAudioSource = owner->GetParent()->GetComponent<ComponentAudioSource>();
	rigidBody->SetKpForce(50);
}

void MeleeHeavyAttackBehaviourScript::Update(float deltaTime)
{
	if (parentHealthSystem->GetCurrentHealth() <= 10.0f
		&& parentEnemyDroneScript->GetDroneBehaviour() == DroneBehaviours::EXPLOSIONATTACK
		&& attackState == ExplosionState::NOTDEAD)
	{
		SetExplosionPosition( parentEnemyDroneScript->GetSeekTargetPosition());
	}

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
		parentHealthSystem->TakeDamage(explosionDamage);
		owner->GetParent()->GetComponent<ComponentRigidBody>()->SetKpForce(0.5f);
		attackState = ExplosionState::DEAD;
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_TIMER);
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::EXPLOSION);
	}
}

void MeleeHeavyAttackBehaviourScript::SetExplosionPosition(float3 explosionPos)
{
	owner->GetParent()->GetComponent<ComponentRigidBody>()->SetPositionTarget(explosionPos);
	owner->GetParent()->GetComponent<ComponentRigidBody>()->SetKpForce(2.0f);
	attackState = ExplosionState::WAITING_EXPLOSION;
	componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::TIMER);
}

void MeleeHeavyAttackBehaviourScript::UpdateDroneColor()
{
	//When we have tint the color of the drone will change.
}

ExplosionState MeleeHeavyAttackBehaviourScript::IsExploted() const
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