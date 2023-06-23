#pragma once
#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class DroneExplosionState
{
	NOTDEAD,
	WAITTINGEXPLOSION,
	EXPLOTING
};

class ComponentRigidBody;
class ComponentTransform;
class HealthSystem;
class EnemyDroneScript;
class GameObject;

class DroneExplosionAttack : public Script
{
public:
	DroneExplosionAttack();
	~DroneExplosionAttack() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetExplosionPosition(float3 explosionPos);
	void UpdateDroneColor();

	DroneExplosionState IsExploted() const;

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	DroneExplosionState attackState;

	GameObject* targetPlayer;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	HealthSystem* parentHealthSystem;
	EnemyDroneScript* parentEnemyDroneScript;

	float explosionDamage;
	float explosionTime;
};
