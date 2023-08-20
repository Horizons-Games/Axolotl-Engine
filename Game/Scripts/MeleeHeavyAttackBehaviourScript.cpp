#include "MeleeHeavyAttackBehaviourScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/SeekBehaviourScript.h"

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
	componentAudioSource = owner->GetParent()->GetComponent<ComponentAudioSource>();
	ownerAgent = owner->GetParent()->GetComponentInternal<ComponentAgent>();
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

		parentHealthSystem->TakeDamage(explosionDamage);
		
		if (ownerAgent)
		{
			ownerAgent->SetMaxAcceleration(ownerAgent->GetMaxAcceleration() / 2.0f);
		}
		else
		{
			//set new target position
			owner->GetParent()->GetComponent<ComponentRigidBody>()->SetKpForce(0.5f);
		}
		
		attackState = ExplosionState::DEAD;
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_TIMER);
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::EXPLOSION);
	}
}

void MeleeHeavyAttackBehaviourScript::SetExplosionPosition(const float3& explosionPos)
{
	if (attackState != ExplosionState::NOTDEAD)
	{
		return;
	}

	if (ownerAgent)
	{
		ownerAgent->SetMoveTarget(explosionPos);
		ownerAgent->SetMaxAcceleration(ownerAgent->GetMaxAcceleration() * 2.0f);
	}
	else
	{
		//set new target position
		ComponentRigidBody* ownerRB = owner->GetParent()->GetComponent<ComponentRigidBody>();
		ownerRB->SetPositionTarget(explosionPos);
		ownerRB->SetKpForce(2.0f);
	}

	attackState = ExplosionState::WAITING_EXPLOSION;
	componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::TIMER);
}

void MeleeHeavyAttackBehaviourScript::UpdateDroneColor()
{
	//When we have tint the color of the drone will change.
}

void MeleeHeavyAttackBehaviourScript::TriggerExplosion()
{
	SeekBehaviourScript* enemySeekBehaviour = owner->GetParent()->GetComponent<SeekBehaviourScript>();
	float3 targetPos = enemySeekBehaviour->GetTarget()->GetComponent<ComponentTransform>()->GetGlobalPosition();

	SetExplosionPosition(targetPos);
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