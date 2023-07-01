#pragma once
#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class ExplosionState
{
	NOTDEAD,
	WAITING_EXPLOSION,
	EXPLODING,
	DEAD
};

class ComponentRigidBody;
class ComponentTransform;
class ComponentAudioSource;
class HealthSystem;
class EnemyDroneScript;

class MeleeHeavyAttackBehaviourScript : public Script
{
public:
	MeleeHeavyAttackBehaviourScript();
	~MeleeHeavyAttackBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetExplosionPosition(float3 explosionPos);
	void UpdateDroneColor();

	ExplosionState IsExploted() const;

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	ExplosionState attackState;

	GameObject* targetPlayer;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	HealthSystem* parentHealthSystem;
	EnemyDroneScript* parentEnemyDroneScript;
	ComponentAudioSource* componentAudioSource;

	float explosionDamage;
	float explosionTime;
};
